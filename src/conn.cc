/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2008-2013 Drizzle Developer Group
 * Copyright (C) 2008 Eric Day (eday@oddments.org)
 * Copyright (C) 2015 Andreas Bok Andersen (andreas.bok@sociomantic.com)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *
 *     * The names of its contributors may not be used to endorse or
 * promote products derived from this software without specific prior
 * written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */


/**
 * @file
 * @brief Connection Definitions
 */

#include "config.h"
#include "src/common.h"

#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif

#include <limits.h>
#include <cerrno>
#ifdef USE_OPENSSL
#include <openssl/err.h>
#endif

/**
 * @addtogroup drizzle_static Static Connection Declarations
 * @ingroup drizzle_con
 * @{
 */

/**
 * Set socket options for a connection.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 * @return Standard drizzle return value.
 */
static drizzle_return_t _setsockopt(drizzle_st *con);

static void connect_failed_try_next(drizzle_st *con, const char *file, uint line,
  const char *function, const char *msg);

static void __closesocket(socket_t& fd)
{
  if (fd != INVALID_SOCKET)
  {
    (void)shutdown(fd, SHUT_RDWR);
    (void)closesocket(fd);
    fd= INVALID_SOCKET;
  }
}

/** @} */

/*
 * Common Definitions
 */

int drizzle_fd(const drizzle_st *con)
{
  if (con == NULL)
  {
    return -1;
  }

  return con->fd;
}

void drizzle_close(drizzle_st *con)
{
  if (con == NULL)
  {
    return;
  }

#ifdef USE_OPENSSL
  if (con->ssl) {
          if (0 == ERR_peek_error() &&
              !(SSL_SENT_SHUTDOWN & SSL_get_shutdown(con->ssl)))
                  SSL_shutdown(con->ssl);
          SSL_free(con->ssl);
          con->ssl = NULL;
  }
  con->ssl_state = DRIZZLE_SSL_STATE_NONE;
  ERR_clear_error();
#endif

  if (con->fd == INVALID_SOCKET)
  {
    return;
  }

  __closesocket(con->fd);

  con->state.ready= false;
  con->packet_number= 0;
  con->buffer_ptr= con->buffer;
  con->buffer_size= 0;
  con->events= 0;
  con->revents= 0;

  con->clear_state();
}

drizzle_return_t drizzle_set_events(drizzle_st *con, short events)
{
  drizzle_return_t ret;

  if ((con->events | events) == con->events)
  {
    return DRIZZLE_RETURN_OK;
  }

  con->events|= events;

  if (con->event_watch_fn != NULL)
  {
    ret= con->event_watch_fn(con, con->events, con->event_watch_context);

    if (ret != DRIZZLE_RETURN_OK)
      {
        drizzle_close(con);
        return ret;
      }
  }

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_set_revents(drizzle_st *con, short revents)
{
  drizzle_return_t ret;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (revents != 0)
    con->state.io_ready= true;

  con->revents= revents;

  /* Remove external POLLOUT watch if we didn't ask for it. Otherwise we spin
     forever until another POLLIN state change. This is much more efficient
     than removing POLLOUT on every state change since some external polling
     mechanisms need to use a system call to change flags (like Linux epoll). */
  if (revents & POLLOUT && !(con->events & POLLOUT) && con->event_watch_fn != NULL)
  {
    ret= con->event_watch_fn(con, con->events, con->event_watch_context);

    if (ret != DRIZZLE_RETURN_OK)
    {
      drizzle_close(con);
      return ret;
    }
  }

  con->events&= (short)~revents;

  return DRIZZLE_RETURN_OK;
}


const char *drizzle_error(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  return (const char *)con->last_error;
}

uint16_t drizzle_error_code(const drizzle_st *con)
{
  if (con == NULL)
  {
    return 0;
  }

  return con->error_code;
}

const char *drizzle_sqlstate(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  return con->sqlstate;
}

drizzle_options_st *drizzle_options_create(void)
{
  return new (std::nothrow) drizzle_options_st;
}

void drizzle_options_destroy(drizzle_options_st *options)
{
  delete options;
}

void drizzle_socket_set_options(drizzle_options_st *options, int wait_timeout,
                                int keepidle, int keepcnt, int keepintvl)
{
  if (options == NULL)
  {
    return;
  }
  options->wait_timeout = wait_timeout;
  options->keepidle = keepidle;
  options->keepcnt = keepcnt;
  options->keepintvl = keepintvl;
}

void drizzle_socket_set_option(drizzle_st *con, drizzle_socket_option_t option,
  int value)
{
  if (con == NULL)
  {
    return;
  }

  switch (option)
  {
    case DRIZZLE_SOCKET_OPTION_TIMEOUT:
      con->options.wait_timeout = value;
      break;

    case DRIZZLE_SOCKET_OPTION_KEEPIDLE:
      con->options.keepidle = value;
      break;

    case DRIZZLE_SOCKET_OPTION_KEEPCNT:
      con->options.keepcnt = value;
      break;

    case DRIZZLE_SOCKET_OPTION_KEEPINTVL:
      con->options.keepintvl = value;
      break;

    default:
      break;
  }
}

int drizzle_socket_get_option(drizzle_st *con, drizzle_socket_option_t option)
{
  if (con == NULL)
  {
    return -1;
  }

  switch (option)
  {
    case DRIZZLE_SOCKET_OPTION_TIMEOUT:
      return con->options.wait_timeout;

    case DRIZZLE_SOCKET_OPTION_KEEPIDLE:
      return con->options.keepidle;

    case DRIZZLE_SOCKET_OPTION_KEEPCNT:
      return con->options.keepcnt;

    case DRIZZLE_SOCKET_OPTION_KEEPINTVL:
      return con->options.keepintvl;

    default:
      return -1;
  }
}

void drizzle_options_set_non_blocking(drizzle_options_st *options, bool state)
{
  if (options == NULL)
  {
    return;
  }
  options->non_blocking= state;
}

bool drizzle_options_get_non_blocking(drizzle_options_st *options)
{
  if (options == NULL)
  {
    return false;
  }
  return options->non_blocking;
}

void drizzle_options_set_raw_scramble(drizzle_options_st *options, bool state)
{
  if (options == NULL)
  {
    return;
  }
  options->raw_scramble= state;
}

bool drizzle_options_get_raw_scramble(drizzle_options_st *options)
{
  if (options == NULL)
  {
    return false;
  }
  return options->raw_scramble;
}

void drizzle_options_set_found_rows(drizzle_options_st *options, bool state)
{
  if (options == NULL)
  {
    return;
  }
  options->found_rows= state;
}

bool drizzle_options_get_found_rows(drizzle_options_st *options)
{
  if (options == NULL)
  {
    return false;
  }
  return options->found_rows;
}

void drizzle_options_set_interactive(drizzle_options_st *options, bool state)
{
  if (options == NULL)
  {
    return;
  }
  options->interactive= state;
}

bool drizzle_options_get_interactive(drizzle_options_st *options)
{
  if (options == NULL)
  {
    return false;
  }
  return options->interactive;
}

void drizzle_options_set_multi_statements(drizzle_options_st *options, bool state)
{
  if (options == NULL)
  {
    return;
  }
  options->multi_statements= state;
}

bool drizzle_options_get_multi_statements(drizzle_options_st *options)
{
  if (options == NULL)
  {
    return false;
  }
  return options->multi_statements;
}

void drizzle_options_set_auth_plugin(drizzle_options_st *options, bool state)
{
  if (options == NULL)
  {
    return;
  }
  options->auth_plugin= state;
}

bool drizzle_options_get_auth_plugin(drizzle_options_st *options)
{
  if (options == NULL)
  {
    return false;
  }
  return options->auth_plugin;
}

void drizzle_options_set_socket_owner(drizzle_options_st *options,
                   drizzle_socket_owner_t owner)
{
  if (options == NULL)
  {
    return;
  }
  options->socket_owner = owner;
}

drizzle_socket_owner_t drizzle_options_get_socket_owner(drizzle_options_st *options)
{
  if (options == NULL)
  {
    return DRIZZLE_SOCKET_OWNER_NATIVE;
  }

  return options->socket_owner;
}

const char *drizzle_host(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  if (con->socket_type == DRIZZLE_CON_SOCKET_TCP)
  {
    if (con->socket.tcp.host == NULL)
      return DRIZZLE_DEFAULT_TCP_HOST;

    return con->socket.tcp.host;
  }

  return NULL;
}

in_port_t drizzle_port(const drizzle_st *con)
{
  if (con && con->socket_type == DRIZZLE_CON_SOCKET_TCP)
  {
    if (con->socket.tcp.port != 0)
    {
      return con->socket.tcp.port;
    }

    return DRIZZLE_DEFAULT_TCP_PORT;
  }

  return 0;
}

void drizzle_set_tcp(drizzle_st *con, const char *host, in_port_t port)
{
  if (con == NULL)
  {
    return;
  }

  con->socket_type= DRIZZLE_CON_SOCKET_TCP;
  con->socket.tcp.addrinfo= NULL;
  drizzle_reset_addrinfo(con);

  if (host == NULL)
  {
    con->socket.tcp.port= 0;
    con->socket.tcp.host= NULL;
  }
  else
  {
    con->socket.tcp.host= con->socket.tcp.host_buffer;
    strncpy(con->socket.tcp.host, host, NI_MAXHOST);
    con->socket.tcp.host[NI_MAXHOST - 1]= 0;
  }

  con->socket.tcp.port= port;
}

const char *drizzle_user(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  return con->user;
}

void drizzle_set_auth(drizzle_st *con, const char *user,
                          const char *password)
{
  if (con == NULL)
  {
    return;
  }

  if (user == NULL)
  {
    con->user[0]= 0;
  }
  else
  {
    strncpy(con->user, user, DRIZZLE_MAX_USER_SIZE);
    con->user[DRIZZLE_MAX_USER_SIZE - 1]= 0;
  }

  if (password == NULL)
  {
    con->password[0]= 0;
  }
  else
  {
    strncpy(con->password, password, DRIZZLE_MAX_PASSWORD_SIZE);
    con->password[DRIZZLE_MAX_PASSWORD_SIZE - 1]= 0;
  }
}

const char *drizzle_db(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  return con->db;
}

void drizzle_set_db(drizzle_st *con, const char *db)
{
  if (con == NULL)
  {
    return;
  }

  if (db == NULL)
  {
    con->db[0]= 0;
  }
  else
  {
    strncpy(con->db, db, DRIZZLE_MAX_DB_SIZE);
    con->db[DRIZZLE_MAX_DB_SIZE - 1]= 0;
  }
}

void *drizzle_context(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  return con->context;
}

void drizzle_set_context(drizzle_st *con, void *context)
{
  if (con == NULL)
  {
    return;
  }

  con->context= context;
}

void drizzle_set_context_free_fn(drizzle_st *con,
                                     drizzle_context_free_fn *function)
{
  if (con == NULL)
  {
    return;
  }

  con->context_free_fn= function;
}

uint8_t drizzle_protocol_version(const drizzle_st *con)
{
  if (con == NULL)
  {
    return 0;
  }

  return con->protocol_version;
}

const char *drizzle_server_version(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  return con->server_version;
}

uint32_t drizzle_server_version_number(const drizzle_st *con)
{
  if (con)
  {
    const char *current= con->server_version;
    char *end;

    uint32_t major= (uint32_t)strtoul(current, &end, 10);
    current= end +1;
    uint32_t minor= (uint32_t)strtoul(current, &end, 10);
    current= end +1;
    uint32_t version= (uint32_t)strtoul(current, &end, 10);

    return (major * 10000) +(minor * 100) +version;
  }

  return 0;
}

uint32_t drizzle_thread_id(const drizzle_st *con)
{
  if (con == NULL)
  {
    return 0;
  }

  return con->thread_id;
}

const unsigned char *drizzle_scramble(const drizzle_st *con)
{
  if (con == NULL)
  {
    return NULL;
  }

  return con->scramble;
}

drizzle_capabilities_t drizzle_capabilities(const drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_CAPABILITIES_NONE;
  }

  return drizzle_capabilities_t(con->capabilities);
}

drizzle_charset_t drizzle_charset(const drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_CHARSET_NONE;
  }

  return con->charset;
}

drizzle_status_t drizzle_status(const drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_CON_STATUS_NONE;
  }

  return con->status;
}

uint32_t drizzle_max_packet_size(const drizzle_st *con)
{
  if (con == NULL)
  {
    return 0;
  }

  return con->max_packet_size;
}

/*
 * Client Definitions
 */

drizzle_return_t drizzle_connect(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  __LOG_LOCATION__

  if (con->state.ready)
  {
    return DRIZZLE_RETURN_OK;
  }

  if (con->has_state())
  {
    if (con->state.raw_packet == false)
    {
      con->push_state(drizzle_state_handshake_server_read);
      con->push_state(drizzle_state_packet_read);
    }

    con->push_state(drizzle_state_connect);
    con->push_state(drizzle_state_addrinfo);
  }

  while (1)
  {
    drizzle_return_t ret = drizzle_state_loop(con);

    if (ret == DRIZZLE_RETURN_IO_WAIT && !con->options.non_blocking) {
      ret = drizzle_wait(con);
      if (ret != DRIZZLE_RETURN_OK)
        return ret;
      continue;
    }

    return ret;
  }
}

drizzle_return_t drizzle_quit(drizzle_st *con)
{
  if (con != NULL)
  {
    drizzle_log_debug(con, __FILE_LINE_FUNC__, "shutting down the connection");
    con->flags.is_shutdown= true;
    drizzle_return_t ret;
    drizzle_result_st *result;
    result= drizzle_command_write(con, NULL, DRIZZLE_COMMAND_QUIT, NULL, 0, 0, &ret);
    drizzle_result_free(result);
    drizzle_free(con);

    if (ret == DRIZZLE_RETURN_LOST_CONNECTION)
    {
      return DRIZZLE_RETURN_OK;
    }

    return ret;
  }

  return DRIZZLE_RETURN_INVALID_ARGUMENT;
}

drizzle_return_t drizzle_select_db(drizzle_st *con,
                                         const char *db)
{
  drizzle_result_st *result;
  drizzle_return_t ret;

  if (db == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_set_db(con, db);
  result= drizzle_command_write(con, NULL, DRIZZLE_COMMAND_INIT_DB,
                                   db, strlen(db), strlen(db), &ret);
  drizzle_result_free(result);
  return ret;
}

drizzle_result_st *drizzle_shutdown(drizzle_st *con,
                                        drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused;
  }

  return drizzle_command_write(con, NULL, DRIZZLE_COMMAND_SHUTDOWN,
                                   "0", 1, 1, ret_ptr);
}

drizzle_result_st *drizzle_kill(drizzle_st *con,
                                uint32_t connection_id,
                                drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused;
  }

  uint32_t sent= htonl(connection_id);
  return drizzle_command_write(con, NULL, DRIZZLE_COMMAND_PROCESS_KILL,
                                   &sent, sizeof(uint32_t), sizeof(uint32_t), ret_ptr);
}

drizzle_result_st *drizzle_ping(drizzle_st *con,
                                    drizzle_return_t *ret_ptr)
{
  return drizzle_command_write(con, NULL, DRIZZLE_COMMAND_PING, NULL, 0,
                                   0, ret_ptr);
}

drizzle_result_st *drizzle_command_write(drizzle_st *con,
                                             drizzle_result_st *result,
                                             drizzle_command_t command,
                                             const void *data, size_t size,
                                             size_t total,
                                             drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused;
  }

  if (con == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return NULL;
  }

  drizzle_result_st *old_result;

  if (con->state.ready == false)
  {
    if (con->state.raw_packet)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "connection not ready");
      *ret_ptr= DRIZZLE_RETURN_NOT_READY;
      return result;
    }

    *ret_ptr= drizzle_connect(con);
    if (*ret_ptr != DRIZZLE_RETURN_OK)
    {
      return result;
    }
  }

  if (con->has_state())
  {
    if (con->state.raw_packet || con->state.no_result_read)
    {
      con->result= NULL;
    }
    else
    {
      for (old_result= con->result_list; old_result != NULL; old_result= old_result->next)
      {
        if (result == old_result)
        {
          drizzle_set_error(con, __FILE_LINE_FUNC__, "result struct already in use");
          *ret_ptr= DRIZZLE_RETURN_INTERNAL_ERROR;
          return result;
        }
      }

      con->result= drizzle_result_create(con);
      if (con->result == NULL)
      {
        *ret_ptr= DRIZZLE_RETURN_MEMORY;
        return NULL;
      }
    }

    con->command= command;
    con->command_data= (unsigned char *)data;
    con->command_size= size;
    con->command_offset= 0;
    con->command_total= total;

    con->push_state(drizzle_state_command_write);
  }
  else if (con->command_data == NULL)
  {
    con->command_data= (unsigned char *)data;
    con->command_size= size;
  }

  *ret_ptr= drizzle_state_loop(con);
  if (*ret_ptr == DRIZZLE_RETURN_PAUSE)
  {
    *ret_ptr= DRIZZLE_RETURN_OK;
  }
  else if (*ret_ptr != DRIZZLE_RETURN_OK &&
           *ret_ptr != DRIZZLE_RETURN_IO_WAIT &&
           *ret_ptr != DRIZZLE_RETURN_ERROR_CODE)
  {
    drizzle_result_free(con->result);
    con->result= result;
  }

  return con->result;
}

/*
 * Local Definitions
 */

void drizzle_reset_addrinfo(drizzle_st *con)
{
  if (con == NULL)
  {
    return;
  }

  switch (con->socket_type)
  {
  case DRIZZLE_CON_SOCKET_TCP:
    if (con->socket.tcp.addrinfo != NULL)
    {
      freeaddrinfo(con->socket.tcp.addrinfo);
      con->socket.tcp.addrinfo= NULL;
    }
    break;

  case DRIZZLE_CON_SOCKET_UDS:
    con->socket.uds.path_buffer[0]= 0;
    break;

  default:
    break;
  }

  con->addrinfo_next= NULL;
}

/*
 * State Definitions
 */

drizzle_return_t drizzle_state_addrinfo(drizzle_st *con)
{
  drizzle_tcp_st *tcp;
  struct addrinfo ai;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  __LOG_LOCATION__

  switch (con->socket_type)
  {
  case DRIZZLE_CON_SOCKET_TCP:
    {
      tcp= &(con->socket.tcp);

      if (tcp->addrinfo != NULL)
      {
        freeaddrinfo(tcp->addrinfo);
        tcp->addrinfo= NULL;
      }

      char port[NI_MAXSERV];
      if (tcp->port != 0)
      {
        snprintf(port, NI_MAXSERV, "%u", tcp->port);
      }
      else
      {
        snprintf(port, NI_MAXSERV, "%u", DRIZZLE_DEFAULT_TCP_PORT);
      }
      port[NI_MAXSERV-1]= 0;

      memset(&ai, 0, sizeof(struct addrinfo));
      ai.ai_socktype= SOCK_STREAM;
      ai.ai_protocol= IPPROTO_TCP;
      ai.ai_family= AF_UNSPEC;

      const char *host;
      if (tcp->host == NULL)
      {
        host= DRIZZLE_DEFAULT_TCP_HOST;
      }
      else
      {
        host= tcp->host;
      }

      drizzle_log_debug(con, __FILE_LINE_FUNC__, "host=%s port=%s", host, port);
      int ret= getaddrinfo(host, port, &ai, &(tcp->addrinfo));
      if (ret != 0)
      {
        drizzle_set_error(con, __FILE_LINE_FUNC__, "getaddrinfo:%s", gai_strerror(ret));
        return DRIZZLE_RETURN_GETADDRINFO;
      }

      con->addrinfo_next= tcp->addrinfo;
    }

    break;

  case DRIZZLE_CON_SOCKET_UDS:
    break;

  default:
    break;
  }

  con->pop_state();

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_connect(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  __LOG_LOCATION__

  __closesocket(con->fd);

  if (con->socket_type == DRIZZLE_CON_SOCKET_UDS)
  {
#if defined _WIN32 || defined __CYGWIN__
    return DRIZZLE_RETURN_COULD_NOT_CONNECT;
#else // defined _WIN32 || defined __CYGWIN__
    if ((con->fd= socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
      con->last_errno= errno;
      return DRIZZLE_RETURN_COULD_NOT_CONNECT;
    }

    struct sockaddr_un servAddr;

    memset(&servAddr, 0, sizeof (struct sockaddr_un));
    servAddr.sun_family= AF_UNIX;
    strncpy(servAddr.sun_path, con->socket.uds.path_buffer, sizeof(servAddr.sun_path)); /* Copy filename */

    do {
      if (connect(con->fd, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0)
      {
        switch (errno)
        {
        case EINPROGRESS:
        case EALREADY:
        case EINTR:
          continue;

        case EISCONN: /* We were spinning waiting on connect */
          {
            break;
          }

        default:
          con->last_errno= errno;
          return DRIZZLE_RETURN_COULD_NOT_CONNECT;
        }
      }
    } while (0);

    con->pop_state();

    return DRIZZLE_RETURN_OK;
#endif // defined _WIN32 || defined __CYGWIN__
  }
  else
  {
    drizzle_return_t dret;
    if (con->addrinfo_next == NULL)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "could not connect");
      con->clear_state();
      return DRIZZLE_RETURN_COULD_NOT_CONNECT;
    }

    {
      int type= con->addrinfo_next->ai_socktype;

      /* Linuxisms to set some fd flags at the same time as creating the socket. */
#ifdef SOCK_CLOEXEC
      type|= SOCK_CLOEXEC;
#endif

#ifdef SOCK_NONBLOCK
      type|= SOCK_NONBLOCK;
#endif

      con->fd= socket(con->addrinfo_next->ai_family,
                      type,
                      con->addrinfo_next->ai_protocol);
    }

    if (con->fd == INVALID_SOCKET)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "socket:%s", strerror(errno));
      con->last_errno= errno;
      return DRIZZLE_RETURN_COULD_NOT_CONNECT;
    }

    dret= _setsockopt(con);
    if (dret != DRIZZLE_RETURN_OK)
    {
      con->last_errno= errno;
      return DRIZZLE_RETURN_COULD_NOT_CONNECT;
    }

    while (1)
    {
      int ret= connect(con->fd, con->addrinfo_next->ai_addr, con->addrinfo_next->ai_addrlen);

#if defined _WIN32 || defined __CYGWIN__
      errno= translate_windows_error();
#endif /* _WIN32 */

      drizzle_log_debug(con, __FILE_LINE_FUNC__, "connect return=%d errno=%s",
                        ret, strerror(errno));

      if (ret == 0)
      {
        /* Success. */
        con->revents= POLLOUT;
        con->addrinfo_next= NULL;
        break;
      }

      con->revents= 0;

      if (errno == EINTR
#ifdef ERESTART
          || errno == ERESTART
#endif
#ifdef EINPROGRESS
          || errno == EINPROGRESS
#endif
#ifdef EALREADY
          || errno == EALREADY
#endif
          )
        {
        /* EINTR: Interrupted system call. POSIX states that in this case,
           the connection attempt will continue asynchronously, as if we'd
           made a nonblocking connect.

           ERESTART: Should never escape libc, but if it does, treat it
           as EINTR.

           EINPROGRESS: Normal response indicating nonblocking connect has been
           started.

           EALREADY: Indicates nonblocking connect already in progress.
        */
          con->pop_state();
        con->revents= 0;
        drizzle_set_events(con, POLLOUT);
        con->push_state(drizzle_state_connecting);
          return DRIZZLE_RETURN_OK;
        }

      if (errno == ECONNREFUSED || errno == ECONNRESET
#ifdef ENETUNREACH
          || errno == ENETUNREACH || errno == EHOSTUNREACH || errno == ENETDOWN
#endif
#ifdef ETIMEDOUT
          || errno == ETIMEDOUT
#endif
#ifdef EAFNOSUPPORT
          || errno == EAFNOSUPPORT
#endif
          )
      {
        /* Positive failure. Try the next address, if it is available. */
        if (con->addrinfo_next == nullptr)
        {
          return DRIZZLE_RETURN_COULD_NOT_CONNECT;
        }

        con->addrinfo_next= con->addrinfo_next->ai_next;
        return DRIZZLE_RETURN_OK;
      }

      /* Other failures not specific to this address */
      drizzle_set_error(con, __FILE_LINE_FUNC__, "connect:%s", strerror(errno));
      con->last_errno= errno;
      return DRIZZLE_RETURN_COULD_NOT_CONNECT;
    }
#ifdef USE_OPENSSL
    if (con->ssl_context) {
	    con->ssl_state = DRIZZLE_SSL_STATE_NONE;
            con->ssl = SSL_new(con->ssl_context);
            if (NULL == con->ssl) {
                    drizzle_set_error(con, __FILE_LINE_FUNC__,
                                      "Could not build SSL object");
                    return DRIZZLE_RETURN_SSL_ERROR;
            }
            ERR_clear_error();
            SSL_set_fd(con->ssl, con->fd);
    }
#endif
    con->pop_state();
  }

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_connecting(drizzle_st *con)
{
  drizzle_return_t ret;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  __LOG_LOCATION__

  if (con->revents & (POLLOUT | POLLERR | POLLHUP))
  {
    int error= 0;
      con->pop_state();
      socklen_t error_length= sizeof(error);
      int getsockopt_error;
    if ((getsockopt_error= getsockopt(con->fd, SOL_SOCKET, SO_ERROR, (void *)&error, &error_length)) < 0)
      {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "getsockopt: %s", strerror(errno));
      return DRIZZLE_RETURN_INTERNAL_ERROR;
      }

    if (error == 0 && (con->revents & POLLOUT))
      {
        /* Successful connection! */
        con->addrinfo_next= NULL;
        if (con->ssl_context) {
		con->ssl_state = DRIZZLE_SSL_STATE_NONE;
                con->ssl = SSL_new(con->ssl_context);
                if (NULL == con->ssl) {
                        drizzle_set_error(con, __FILE_LINE_FUNC__, "Could not "
                                          "build SSL object");
                        return DRIZZLE_RETURN_SSL_ERROR;
                }
                ERR_clear_error();
                SSL_set_fd(con->ssl, con->fd);
        }
        return DRIZZLE_RETURN_OK;
      }
    else
    {
      const char *msg;

      if (error != 0)
      {
        /* If we have an error code from SO_ERROR,
           report it in the failure message */
        msg= strerror(error);
      }
      else if (con->revents & POLLHUP)
      {
        /* If we don't have an error code, but we know the connection
           was dropped, just report that */
        msg= "Hangup";
    }
      else
    {
        /* We only reach here if we get POLLERR from poll(), but
           SO_ERROR returned zero. Presumably shouldn't happen. */
        msg= "Unknown failure";
    }

      /* Failed connection. Try the next address. */
      connect_failed_try_next(con, __FILE_LINE_FUNC__, msg);
      return DRIZZLE_RETURN_OK;
    }
  }
  else if (con->revents & POLLNVAL)
    {
    /* Failed connection. Try the next address. */
      con->revents= 0;
      con->pop_state();
    connect_failed_try_next(con, __FILE_LINE_FUNC__, "Invalid file descriptor");
      return DRIZZLE_RETURN_OK;
  } else {
    con->revents= 0;
    ret= drizzle_set_events(con, POLLOUT);
    if (ret != DRIZZLE_RETURN_OK)
    {
      return ret;
    }
    else
    {
      return DRIZZLE_RETURN_IO_WAIT;
    }
  }
}

drizzle_return_t drizzle_state_read(drizzle_st *con)
{
  drizzle_return_t ret;
  ssize_t read_size;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  __LOG_LOCATION__

  if (con->buffer_size == 0)
  {
    con->buffer_ptr= con->buffer;
  }
  else if ((size_t)(con->buffer_ptr - con->buffer) > (con->buffer_allocation / 2))
  {
    memmove(con->buffer, con->buffer_ptr, con->buffer_size);
    con->buffer_ptr= con->buffer;
  }

  if ((con->revents & POLLIN) == 0 &&
      (con->options.non_blocking))
  {
    /* non-blocking mode: return IO_WAIT instead of attempting to read. This
     * avoids reading immediately after writing a command, which typically
     * returns EAGAIN. This improves performance. */
    ret= drizzle_set_events(con, POLLIN);
    if (ret != DRIZZLE_RETURN_OK)
    {
      return ret;
    }

    return DRIZZLE_RETURN_IO_WAIT;
  }

  assert(con->buffer_allocation > 0); /* Appease static analyzer */

  while (1)
  {
    size_t available_buffer= con->buffer_allocation - ((size_t)(con->buffer_ptr - con->buffer) + con->buffer_size);
    if (available_buffer == 0)
    {
      if (con->buffer_allocation >= DRIZZLE_MAX_BUFFER_SIZE)
      {
        drizzle_set_error(con, __FILE_LINE_FUNC__,
                          "buffer too small:%" PRIu32 , con->packet_size + 4);
        return DRIZZLE_RETURN_INTERNAL_ERROR;
      }
      // Shift data to beginning of the buffer then resize
      // This means that buffer_ptr isn't screwed up by realloc pointer move
      if (con->buffer_ptr != con->buffer)
      {
        memmove(con->buffer, con->buffer_ptr, con->buffer_size);
      }
      con->buffer_allocation= con->buffer_allocation * 2;
      unsigned char *realloc_buffer= (unsigned char*)realloc(con->buffer, con->buffer_allocation);
      if (realloc_buffer == NULL)
      {
        drizzle_set_error(con, __FILE_LINE_FUNC__, "realloc failure");
        return DRIZZLE_RETURN_MEMORY;
      }
      con->buffer= realloc_buffer;
      drizzle_log_debug(con, __FILE_LINE_FUNC__, "buffer resized to: %" PRIu32, con->buffer_allocation);
      con->buffer_ptr= con->buffer;
      available_buffer= con->buffer_allocation - con->buffer_size;
    }

#ifdef USE_OPENSSL
    if (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE)
    {
        ERR_clear_error();
        read_size= SSL_read(con->ssl, (char*)con->buffer_ptr + con->buffer_size, (available_buffer % INT_MAX));
        if (read_size <= 0) {
                int rc = SSL_get_error(con->ssl, read_size);
                drizzle_return_t rsev;
                switch (rc) {
                case SSL_ERROR_WANT_X509_LOOKUP:
                case SSL_ERROR_WANT_READ:
                        con->revents &= ~POLLIN;
                        rsev = drizzle_set_events(con, POLLIN);
                        return DRIZZLE_RETURN_OK == rsev ?
                                DRIZZLE_RETURN_IO_WAIT : rsev;
                case SSL_ERROR_WANT_WRITE:
                        con->revents = 0;
                        rsev = drizzle_set_events(con, POLLOUT);
                        return DRIZZLE_RETURN_OK == rsev ?
                                DRIZZLE_RETURN_IO_WAIT : rsev;
                case SSL_ERROR_ZERO_RETURN:
                        // peer closed connection
                        ERR_clear_error();
                        SSL_shutdown(con->ssl);
                        return DRIZZLE_RETURN_LOST_CONNECTION;
                case SSL_ERROR_SYSCALL:
                        drizzle_set_error(con, __FILE_LINE_FUNC__,
                                          "SSL error: %s", strerror(errno));
                        return DRIZZLE_RETURN_SSL_ERROR;
                case SSL_ERROR_SSL:
                        // TODO
                        drizzle_set_error(con, __FILE_LINE_FUNC__,
                                          "SSL error: %d", rc);
                        return DRIZZLE_RETURN_SSL_ERROR;
                default:
                        drizzle_set_error(con, __FILE_LINE_FUNC__,
                                          "SSL error: %d", rc);
                        return DRIZZLE_RETURN_SSL_ERROR;
                }
        }
    }
    else
#endif
    {
      read_size= recv(con->fd, (char *)con->buffer_ptr + con->buffer_size, available_buffer, MSG_NOSIGNAL);
    }

#if defined _WIN32 || defined __CYGWIN__
    errno= translate_windows_error();
#endif // defined _WIN32 || defined __CYGWIN__

    drizzle_log_debug(con, __FILE_LINE_FUNC__,
                      "read fd=%d avail= %" PRIu64 " recv=%" PRIi32 " ssl= %d errno=%s",
                      con->fd, available_buffer, read_size,
                      (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE) ? 1 : 0,
                      strerror(errno));

    if (read_size == 0)
    {
      if (con->flags.is_shutdown == false)
      {
        drizzle_set_error(con, __FILE_LINE_FUNC__, "lost connection to server (EOF)");
      }

      return DRIZZLE_RETURN_LOST_CONNECTION;
    }
    else if (read_size == -1)
    {
      // switch (get_socket_errno())
      switch (errno)
      {
      case EAGAIN:
#if defined(EWOULDBLOCK) && (EWOULDBLOCK != EAGAIN)
      case EWOULDBLOCK:
#endif
        {
          /* clear the read ready flag */
          con->revents&= ~POLLIN;
          ret= drizzle_set_events(con, POLLIN);
          if (ret != DRIZZLE_RETURN_OK)
          {
            return ret;
          }

          if (con->options.non_blocking)
          {
            return DRIZZLE_RETURN_IO_WAIT;
          }

          ret= drizzle_wait(con);
          if (ret != DRIZZLE_RETURN_OK)
          {
            return ret;
          }

          continue;
        }
      case ECONNREFUSED:
        {
          con->revents= 0;
          con->pop_state();
          con->push_state(drizzle_state_connect);

          /* Positive failure. Try the next address, if available. */
          if (con->addrinfo_next == nullptr)
          {
            return DRIZZLE_RETURN_COULD_NOT_CONNECT;
          }

          con->addrinfo_next= con->addrinfo_next->ai_next;
          return DRIZZLE_RETURN_OK;
        }

      case EINTR:
        continue;

      case EINVAL:
        {
          drizzle_log_debug(con, __FILE_LINE_FUNC__,
                            "EINVAL fd=%d buffer=%p available_buffer=%" PRIu64,
                            con->fd, (char *)con->buffer_ptr + con->buffer_size,
                            available_buffer);
        }
        break;

      case EPIPE:
      case ECONNRESET:
        {
          drizzle_set_error(con, __FILE_LINE_FUNC__,
                            "lost connection to server (%s)", strerror(errno));
          return DRIZZLE_RETURN_LOST_CONNECTION;
        }
      }

      drizzle_set_error(con, __FILE_LINE_FUNC__, "recv:%s", strerror(errno));
      con->last_errno= errno;
      return DRIZZLE_RETURN_ERRNO;
    }

    /* clear the "read ready" flag if we read all available data. */
    if ((size_t) read_size < available_buffer)
    {
      con->revents&= ~POLLIN;
    }
    con->buffer_size+= (size_t)read_size;
    break;
  }

  con->pop_state();

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_write(drizzle_st *con)
{
  drizzle_return_t ret;
  ssize_t write_size;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  __LOG_LOCATION__

  while (con->buffer_size != 0)
  {
#ifdef USE_OPENSSL
    if (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE)
    {
      ERR_clear_error();
      write_size= SSL_write(con->ssl, con->buffer_ptr, (con->buffer_size % INT_MAX));
      if (write_size <= 0) {
              int rc = SSL_get_error(con->ssl, write_size);
              drizzle_return_t rsev;
              switch (rc) {
              case SSL_ERROR_WANT_X509_LOOKUP:
              case SSL_ERROR_WANT_READ:
                      con->revents = 0;
                      rsev = drizzle_set_events(con, POLLIN);
                      return DRIZZLE_RETURN_OK == rsev ?
                              DRIZZLE_RETURN_IO_WAIT : rsev;
              case SSL_ERROR_WANT_WRITE:
                      con->revents &= POLLOUT;
                      rsev = drizzle_set_events(con, POLLOUT);
                      return DRIZZLE_RETURN_OK == rsev ?
                              DRIZZLE_RETURN_IO_WAIT : rsev;
              case SSL_ERROR_ZERO_RETURN:
                      // peer closed connection
                      // We can get this from a SSL_read but I don't think we
                      // can get it from a SSL_write
                      drizzle_set_error(con, __FILE_LINE_FUNC__,
                                        "Peer closed SSL connection");
                      return DRIZZLE_RETURN_LOST_CONNECTION;
              case SSL_ERROR_SYSCALL:
                      drizzle_set_error(con, __FILE_LINE_FUNC__,
                                        "SSL error: %s", strerror(errno));
                      return DRIZZLE_RETURN_SSL_ERROR;
              case SSL_ERROR_SSL:
                      drizzle_set_error(con, __FILE_LINE_FUNC__,
                                        "SSL error: %d", rc);
                      return DRIZZLE_RETURN_SSL_ERROR;
              default:
                      drizzle_set_error(con, __FILE_LINE_FUNC__,
                                        "SSL error: %d", rc);
                      return DRIZZLE_RETURN_SSL_ERROR;
              }
      }
    }
    else
#endif
    {
      write_size= send(con->fd,(char *) con->buffer_ptr, con->buffer_size, MSG_NOSIGNAL);
    }

#if defined _WIN32 || defined __CYGWIN__
    errno= translate_windows_error();
#endif // defined _WIN32 || defined __CYGWIN__

    drizzle_log_debug(con, __FILE_LINE_FUNC__, "write fd=%d return=%" PRIi64 " ssl=%d errno=%s",
                      con->fd, write_size,
                      (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE) ? 1 : 0,
                      strerror(errno));

    if (write_size == 0)
    { }
    else if (write_size == -1)
    {

// workaround for  Werror=logical-op compile time error
// https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69602#c16
#if EAGAIN == EWOULDBLOCK
  #define EAGAIN_OR_WOULDBLOCK(e) (e == EAGAIN)
#else
  #define EAGAIN_OR_WOULDBLOCK(e) (e == EAGAIN || e == EWOULDBLOCK)
#endif
      if ( EAGAIN_OR_WOULDBLOCK(errno) )
      {
        ret= drizzle_set_events(con, POLLOUT);
        if (ret != DRIZZLE_RETURN_OK)
        {
          return ret;
        }

        if (con->options.non_blocking)
        {
          return DRIZZLE_RETURN_IO_WAIT;
        }

        ret= drizzle_wait(con);
        if (ret != DRIZZLE_RETURN_OK)
        {
          return ret;
        }

        continue;
      }
      else if (errno == EINTR)
      {
        continue;
      }
      else if (errno == EPIPE || errno == ECONNRESET)
      {
        drizzle_set_error(con, __FILE_LINE_FUNC__, "lost connection to server (%s)",
                          strerror(errno));
        return DRIZZLE_RETURN_LOST_CONNECTION;
      }

      drizzle_set_error(con, __FILE_LINE_FUNC__, "send: %s", strerror(errno));
      con->last_errno= errno;
      return DRIZZLE_RETURN_ERRNO;
    }

    con->buffer_ptr+= write_size;
    con->buffer_size-= (size_t)write_size;
    if (con->buffer_size == 0)
      break;
  }

  con->buffer_ptr= con->buffer;

  con->pop_state();

  return DRIZZLE_RETURN_OK;
}

/*
 * Static Definitions
 */

static drizzle_return_t _setsockopt(drizzle_st *con)
{
  struct linger linger;
  struct timeval waittime;
  const socklen_t optlen_int = sizeof(int);

#if defined _WIN32
  typedef const char* sockopt_value_t;
  typedef const char* sockopt_linger_t;
  typedef const char* sockopt_timeval_t;
#else
  typedef int* sockopt_value_t ;
  typedef struct linger* sockopt_linger_t;
  typedef struct timeval* sockopt_timeval_t;
#endif

  assert(con);
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

#if defined(HAVE_FCNTL) && !defined(SOCK_CLOEXEC) && defined(FD_CLOEXEC)
  {
    int flags;
    do
    {
      flags= fcntl(con->fd, F_GETFD, 0);
    } while (flags == -1 and (errno == EINTR or errno == EAGAIN));

    if (flags != -1)
    {
      int rval;
      do
      {
        rval= fcntl (con->fd, F_SETFD, flags | FD_CLOEXEC);
      } while (rval == -1 && (errno == EINTR or errno == EAGAIN));
      // we currently ignore the case where rval is -1
    }
  }
#endif // HAVE_FCNTL but not SOCK_CLOEXEC


  int ret= 1;

  ret= setsockopt(con->fd, IPPROTO_TCP, TCP_NODELAY, (sockopt_value_t)&ret,
    optlen_int);

  if (ret == -1 && errno != EOPNOTSUPP)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:TCP_NODELAY:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  linger.l_onoff= 1;
  linger.l_linger= con->options.wait_timeout;

  ret= setsockopt(con->fd, SOL_SOCKET, SO_LINGER, (sockopt_linger_t)&linger,
                  (socklen_t)sizeof(struct linger));

  if (ret == -1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:SO_LINGER:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  waittime.tv_sec= con->options.wait_timeout;
  waittime.tv_usec= 0;

  ret= setsockopt(con->fd, SOL_SOCKET, SO_SNDTIMEO, (sockopt_timeval_t)&waittime,
                  (socklen_t)sizeof(struct timeval));

  if (ret == -1 && errno != ENOPROTOOPT)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:SO_SNDTIMEO:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  ret= setsockopt(con->fd, SOL_SOCKET, SO_RCVTIMEO, (sockopt_timeval_t)&waittime,
                  (socklen_t)sizeof(struct timeval));

  if (ret == -1 && errno != ENOPROTOOPT)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "setsockopt:SO_RCVTIMEO:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  ret = 1;

#ifdef HAVE_SO_KEEPALIVE
  ret= setsockopt(con->fd, SOL_SOCKET, SO_KEEPALIVE, (sockopt_value_t)&ret,
    optlen_int);

  if (ret == -1 && errno != ENOPROTOOPT)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "setsockopt:SO_KEEPALIVE:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#endif /* HAVE_SO_KEEPALIVE */

#ifdef HAVE_TCP_KEEPIDLE
  ret= setsockopt(con->fd, IPPROTO_TCP, TCP_KEEPIDLE,
                  (sockopt_value_t)&con->options.keepidle, optlen_int);
  if (ret == -1 && errno != EOPNOTSUPP)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:TCP_KEEPIDLE:%s",
                      strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#endif /* HAVE_TCP_KEEPIDLE */

#ifdef HAVE_TCP_KEEPCNT
  ret= setsockopt(con->fd, IPPROTO_TCP, TCP_KEEPCNT,
                  (sockopt_value_t)&con->options.keepcnt, optlen_int);
  if (ret == -1 && errno != EOPNOTSUPP)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:TCP_KEEPCNT:%s",
                      strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#endif /* HAVE_TCP_KEEPCNT */

#ifdef HAVE_TCP_KEEPINTVL
  ret= setsockopt(con->fd, IPPROTO_TCP, TCP_KEEPINTVL,
                  (sockopt_value_t)&con->options.keepintvl, optlen_int);


  if (ret == -1 && errno != EOPNOTSUPP)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:TCP_KEEPINTVL:%s",
                      strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#endif /* HAVE_TCP_KEEPINTVL */

  ret= DRIZZLE_DEFAULT_SOCKET_SEND_SIZE;
  ret= setsockopt(con->fd, SOL_SOCKET, SO_SNDBUF, (sockopt_value_t)&ret, optlen_int);
  if (ret == -1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:SO_SNDBUF:%s",
                      strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  ret= DRIZZLE_DEFAULT_SOCKET_RECV_SIZE;
  ret= setsockopt(con->fd, SOL_SOCKET, SO_RCVBUF, (sockopt_value_t)&ret, optlen_int);
  if (ret == -1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt:SO_RCVBUF:%s",
                      strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

#if defined(SO_NOSIGPIPE)
  ret= setsockopt(con->fd, SOL_SOCKET, SO_NOSIGPIPE, static_cast<void *>(&ret),
                  optlen_int);

  if (ret == -1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "setsockopt(SO_NOSIGPIPE): %s",
                      strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#elif defined(HAVE_FCNTL) && defined(F_SETNOSIGPIPE)
  ret= fcntl(con->fd, F_SETNOSIGPIPE, 1);

  if (ret == -1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "fcntl:F_SETNOSIGPIPE:%s",
                      strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#endif

#if defined (_WIN32)
  {
    unsigned long asyncmode;
    asyncmode= 1;
    ioctlsocket(con->fd, FIONBIO, &asyncmode);
  }
#else
#if HAVE_FCNTL && defined(O_NONBLOCK) && !defined(SOCK_NONBLOCK)
  ret= fcntl(con->fd, F_GETFL, 0);
  if (ret == -1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "fcntl:F_GETFL:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  ret= fcntl(con->fd, F_SETFL, ret | O_NONBLOCK);
  if (ret == -1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "fcntl:F_SETFL:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#endif
#endif

  return DRIZZLE_RETURN_OK;
}

static void connect_failed_try_next(drizzle_st *con, const char *file, uint line,
  const char *function, const char *msg)
{
  char hostbuf[NI_MAXHOST], servbuf[NI_MAXSERV];
  struct addrinfo *aip = con->addrinfo_next;

  if (getnameinfo(aip->ai_addr, aip->ai_addrlen,
                  hostbuf, sizeof(hostbuf),
                  servbuf, sizeof(servbuf),
                  NI_NUMERICHOST | NI_NUMERICSERV)) {
    strcpy(hostbuf, "???");
    strcpy(servbuf, "???");
  }
  drizzle_log_info(con, file, line, function, "connect failure: host=%s port=%s msg=%s",
                   hostbuf, servbuf, msg);

  drizzle_set_error(con, file, line, function, "connect: %s (port %s): %s",
                    hostbuf, servbuf, msg);

  if (con->addrinfo_next != nullptr)
  {
    con->addrinfo_next= con->addrinfo_next->ai_next;
  }

  con->push_state(drizzle_state_connect);
}
