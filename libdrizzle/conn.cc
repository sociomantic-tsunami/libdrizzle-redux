/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2008-2013 Drizzle Developer Group
 * Copyright (C) 2008 Eric Day (eday@oddments.org)
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
#include "libdrizzle/common.h"

#ifndef MSG_NOSIGNAL
# define MSG_NOSIGNAL 0
#endif

#include <cerrno>

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

static void __closesocket(socket_t& fd)
{
  if (fd != INVALID_SOCKET)
  {
    (void)shutdown(fd, SHUT_RDWR);
    (void)closesocket(fd);
    fd= INVALID_SOCKET;
  }
}

static bool connect_poll(drizzle_st *con)
{
  struct pollfd fds[1];
  fds[0].fd= con->fd;
  fds[0].events= POLLIN;

  size_t loop_max= 5; // This should only be used for EINTR
  while (--loop_max) // Should only loop on cases of ERESTART or EINTR
  {
    int error= poll(fds, 1, con->timeout);
    if (error == 1)
    {
      if (fds[0].revents & (POLLIN))
      {
        drizzle_log_debug(con, "poll(POLLIN)");
        return true;
      }

      if (fds[0].revents & (POLLOUT))
      {
        drizzle_log_debug(con, "poll(POLLOUT)");
        return true;
      }

      if (fds[0].revents & (POLLERR | POLLHUP | POLLNVAL))
      {
#ifdef __MINGW32__
        char err;
#else
        int err;
#endif
        socklen_t len= sizeof (err);
        // We replace errno with err if getsockopt() passes, but err has been
        // set.
        if (getsockopt(con->fd, SOL_SOCKET, SO_ERROR, &err, &len) == 0)
        {
          // We check the value to see what happened wth the socket.
          if (err == 0)
          {
            return true;
          }
          errno= err;
          perror("poll()");
        }

        // "getsockopt() failed"
        return false;
      }

      assert(0);

      return false;
    }
    else if (error == 0)
    {
      // "timeout occurred while trying to connect"
      drizzle_log_debug(con, "poll(TIMEOUT) %d", con->timeout);
      return false;
    }

    perror("poll2()");
    switch (get_socket_errno())
    {
#ifdef TARGET_OS_LINUX
    case ERESTART:
#endif
    case EINTR:
      continue;

    case EFAULT:
    case ENOMEM:
      // "poll() failure"
      break;

    case EINVAL:
      // "RLIMIT_NOFILE exceeded, or if OSX the timeout value was invalid"
      break;

    default:
      break;
    }

    //"socket error occurred");
    return false;
  }

  // This should only be possible from ERESTART or EINTR; 
  // "connection failed (error should be from either ERESTART or EINTR"
  return false;
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
  if ((con->events | events) == con->events)
  {
    return DRIZZLE_RETURN_OK;
  }

  con->events|= events;

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_set_revents(drizzle_st *con, short revents)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (revents != 0)
    con->state.io_ready= true;

  con->revents= revents;

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

  return drizzle_state_loop(con);
}

drizzle_return_t drizzle_quit(drizzle_st *con)
{
  if (con != NULL)
  {
    drizzle_log_debug(con, "shutting down the connection");
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
      drizzle_set_error(con, "drizzle_command_write",
                        "connection not ready");
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
          drizzle_set_error(con, "drizzle_command_write", "result struct already in use");
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

  drizzle_log_debug(con, __func__);

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

      drizzle_log_debug(con, "host=%s port=%s", host, port);
      int ret= getaddrinfo(host, port, &ai, &(tcp->addrinfo));
      if (ret != 0)
      {
        drizzle_set_error(con, "drizzle_state_addrinfo", "getaddrinfo:%s", gai_strerror(ret));
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

  drizzle_log_debug(con, "drizzle_state_connect");

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
      drizzle_set_error(con, __func__, "could not connect");
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
      drizzle_set_error(con, __func__, "socket:%s", strerror(errno));
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

      drizzle_log_debug(con, "connect return=%d errno=%s", ret, strerror(errno));

      if (ret == 0)
      {
        con->addrinfo_next= NULL;
        break;
      }

      if (errno == EAGAIN || errno == EINTR)
      {
        continue;
      }

      if (errno == EINPROGRESS)
      {
        if (connect_poll(con))
        {
          con->pop_state();
          return DRIZZLE_RETURN_OK;
        }
      }
      else if (errno == ECONNREFUSED || errno == ENETUNREACH || errno == ETIMEDOUT)
      {
        con->addrinfo_next= con->addrinfo_next->ai_next;
        return DRIZZLE_RETURN_OK;
      }

      drizzle_set_error(con, __func__, "connect:%s", strerror(errno));
      con->last_errno= errno;
      return DRIZZLE_RETURN_COULD_NOT_CONNECT;
    }
#ifdef USE_OPENSSL
    if (con->ssl)
    {
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

  drizzle_log_debug(con, "drizzle_state_connecting");

  while (1)
  {
    int error= 0;
    if (con->revents & POLLOUT)
    {
      con->pop_state();
      socklen_t error_length= sizeof(error);
      int getsockopt_error;
      if ((getsockopt_error= getsockopt(con->fd, SOL_SOCKET, SO_ERROR, (char*)&error, &error_length)) < 1)
      {
        drizzle_set_error(con, __func__, strerror(getsockopt_error));
        return DRIZZLE_RETURN_COULD_NOT_CONNECT;
      }

      if (error == 0)
      {
        con->pop_state();
        return DRIZZLE_RETURN_OK;
      }
    }
    else if (con->revents & (POLLERR | POLLHUP | POLLNVAL))
    {
      error= 1;
    }

    if (error)
    {
      con->revents= 0;
      con->pop_state();
      con->push_state(drizzle_state_connect);
      con->addrinfo_next= con->addrinfo_next->ai_next;

      return DRIZZLE_RETURN_OK;
    }

    ret= drizzle_set_events(con, POLLOUT);
    if (ret != DRIZZLE_RETURN_OK)
    {
      return ret;
    }
    else if (con->options.non_blocking)
    {
      return DRIZZLE_RETURN_IO_WAIT;
    }

    ret= drizzle_wait(con);
    if (ret != DRIZZLE_RETURN_OK)
    {
      return ret;
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

  drizzle_log_debug(con, "drizzle_state_read");

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
        drizzle_set_error(con, __func__,
                          "buffer too small:%zu", con->packet_size + 4);
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
        drizzle_set_error(con, __func__, "realloc failure");
        return DRIZZLE_RETURN_MEMORY;
      }
      con->buffer= realloc_buffer;
      drizzle_log_debug(con, "buffer resized to: %zu", con->buffer_allocation);
      con->buffer_ptr= con->buffer;
      available_buffer= con->buffer_allocation - con->buffer_size;
    }

#ifdef USE_OPENSSL
    if (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE)
    {
      read_size= SSL_read(con->ssl, (char*)con->buffer_ptr + con->buffer_size, available_buffer);
    }
    else
#endif
    {
      read_size= recv(con->fd, (char *)con->buffer_ptr + con->buffer_size, available_buffer, MSG_NOSIGNAL);
    }

#if defined _WIN32 || defined __CYGWIN__
    errno= translate_windows_error();
#endif // defined _WIN32 || defined __CYGWIN__

    drizzle_log_debug(con, "read fd=%d avail= %zd recv=%zd ssl= %d errno=%s",
                      con->fd, available_buffer, read_size, 
                      (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE) ? 1 : 0,
                      strerror(errno));

    if (read_size == 0)
    {
      if (con->flags.is_shutdown == false)
      {
        drizzle_set_error(con, __func__,
                          "%s:%d lost connection to server (EOF)", __FILE__, __LINE__);
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
          con->addrinfo_next= con->addrinfo_next->ai_next;
          return DRIZZLE_RETURN_OK;
        }

      case EINTR:
        continue;

      case EINVAL:
        {
          drizzle_log_debug(con, "EINVAL fd=%d buffer=%p available_buffer=%zd",
                            con->fd, (char *)con->buffer_ptr + con->buffer_size, available_buffer);
        }
        break;

      case EPIPE:
      case ECONNRESET:
        {
          drizzle_set_error(con, __func__,
                            "%s:%d lost connection to server (%s)",
                            __FILE__, __LINE__, strerror(errno));
          return DRIZZLE_RETURN_LOST_CONNECTION;
        }
      }

      drizzle_set_error(con, __func__, "recv:%s", strerror(errno));
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

  drizzle_log_debug(con, "drizzle_state_write");

  while (con->buffer_size != 0)
  {
#ifdef USE_OPENSSL
    if (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE)
    {
      write_size= SSL_write(con->ssl, con->buffer_ptr, con->buffer_size);
    }
    else
#endif      
    {
      write_size= send(con->fd,(char *) con->buffer_ptr, con->buffer_size, MSG_NOSIGNAL);
    }

#if defined _WIN32 || defined __CYGWIN__
    errno= translate_windows_error();
#endif // defined _WIN32 || defined __CYGWIN__

    drizzle_log_debug(con, "write fd=%d return=%zd ssl=%d errno=%s",
                      con->fd, write_size,
                      (con->ssl_state == DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE) ? 1 : 0,
                      strerror(errno));

    if (write_size == 0)
    { }
    else if (write_size == -1)
    {
      if (errno == EAGAIN)
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
        drizzle_set_error(con, __func__, "%s:%d lost connection to server (%s)", 
                          __FILE__, __LINE__, strerror(errno));
        return DRIZZLE_RETURN_LOST_CONNECTION;
      }

      drizzle_set_error(con, "drizzle_state_write", "send: %s", strerror(errno));
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

  assert(con);
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

#if HAVE_FCNTL && !defined(SOCK_CLOEXEC) && defined(FD_CLOEXEC)
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

#ifdef _WIN32
  ret= setsockopt(con->fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&ret, (socklen_t)sizeof(int));
#else
  ret= setsockopt(con->fd, IPPROTO_TCP, TCP_NODELAY, &ret, (socklen_t)sizeof(int));
#endif /* _WIN32 */

  if (ret == -1 && errno != EOPNOTSUPP)
  {
    drizzle_set_error(con, __func__, "setsockopt:TCP_NODELAY:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  linger.l_onoff= 1;
  linger.l_linger= DRIZZLE_DEFAULT_SOCKET_TIMEOUT;

#ifdef _WIN32
  ret= setsockopt(con->fd, SOL_SOCKET, SO_LINGER, (const char*)&linger,
                  (socklen_t)sizeof(struct linger));
#else
  ret= setsockopt(con->fd, SOL_SOCKET, SO_LINGER, &linger,
                  (socklen_t)sizeof(struct linger));
#endif /* _WIN32 */

  if (ret == -1)
  {
    drizzle_set_error(con, __func__, "setsockopt:SO_LINGER:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  waittime.tv_sec= DRIZZLE_DEFAULT_SOCKET_TIMEOUT;
  waittime.tv_usec= 0;

#ifdef _WIN32
  ret= setsockopt(con->fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&waittime,
                  (socklen_t)sizeof(struct timeval));
#else
  ret= setsockopt(con->fd, SOL_SOCKET, SO_SNDTIMEO, &waittime,
                  (socklen_t)sizeof(struct timeval));
#endif /* _WIN32 */

  if (ret == -1 && errno != ENOPROTOOPT)
  {
    drizzle_set_error(con, __func__, "setsockopt:SO_SNDTIMEO:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

#ifdef _WIN32
  ret= setsockopt(con->fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&waittime,
                  (socklen_t)sizeof(struct timeval));
#else
  ret= setsockopt(con->fd, SOL_SOCKET, SO_RCVTIMEO, &waittime,
                  (socklen_t)sizeof(struct timeval));
#endif /* _WIN32 */

  if (ret == -1 && errno != ENOPROTOOPT)
  {
    drizzle_set_error(con, __func__,
                      "setsockopt:SO_RCVTIMEO:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  ret= DRIZZLE_DEFAULT_SOCKET_SEND_SIZE;
#ifdef _WIN32
  ret= setsockopt(con->fd, SOL_SOCKET, SO_SNDBUF, (const char*)&ret, (socklen_t)sizeof(int));
#else
  ret= setsockopt(con->fd, SOL_SOCKET, SO_SNDBUF, &ret, (socklen_t)sizeof(int));
#endif /* _WIN32 */
  if (ret == -1)
  {
    drizzle_set_error(con, __func__, "setsockopt:SO_SNDBUF:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

  ret= DRIZZLE_DEFAULT_SOCKET_RECV_SIZE;
#ifdef _WIN32
  ret= setsockopt(con->fd, SOL_SOCKET, SO_RCVBUF, (const char*)&ret, (socklen_t)sizeof(int));
#else
  ret= setsockopt(con->fd, SOL_SOCKET, SO_RCVBUF, &ret, (socklen_t)sizeof(int));
#endif /* _WIN32 */
  if (ret == -1)
  {
    drizzle_set_error(con, __func__, "setsockopt:SO_RCVBUF:%s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }

#if defined(SO_NOSIGPIPE)
  ret= setsockopt(con->fd, SOL_SOCKET, SO_NOSIGPIPE, static_cast<void *>(&ret), sizeof(int));
  
  if (ret == -1)
  {
    drizzle_set_error(con, __func__, "setsockopt(SO_NOSIGPIPE): %s", strerror(errno));
    return DRIZZLE_RETURN_ERRNO;
  }
#elif HAVE_FCNTL && defined(F_SETNOSIGPIPE)
  ret= fcntl(con->fd, F_SETNOSIGPIPE, 1);
  
  if (ret == -1)
  {
    drizzle_set_error(con, __func__, "fcntl:F_SETNOSIGPIPE:%s", strerror(errno));
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
  // @todo find out why this can't be non-blocking for SSL
  if (!con->ssl)
  {
#if HAVE_FCNTL && defined(O_NONBLOCK) && !defined(SOCK_NONBLOCK)
    ret= fcntl(con->fd, F_GETFL, 0);
    if (ret == -1)
    {
      drizzle_set_error(con, __func__, "fcntl:F_GETFL:%s", strerror(errno));
      return DRIZZLE_RETURN_ERRNO;
    }
    
    ret= fcntl(con->fd, F_SETFL, ret | O_NONBLOCK);
    if (ret == -1)
    {
      drizzle_set_error(con, __func__, "fcntl:F_SETFL:%s", strerror(errno));
      return DRIZZLE_RETURN_ERRNO;
    }
#endif
  }
#endif

  return DRIZZLE_RETURN_OK;
}
