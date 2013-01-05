/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
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
 * @brief Drizzle Definitions
 */

#include "config.h"
#include "libdrizzle/common.h"

#include <cerrno>

/**
 * @addtogroup drizzle_static Static Drizzle Declarations
 * @ingroup drizzle
 * @{
 */

/**
 * Names of the verbose levels provided.
 */
static const char *_verbose_name[DRIZZLE_VERBOSE_MAX]=
{
  "NEVER",
  "CRITICAL",
  "ERROR",
  "INFO",
  "DEBUG",
};

/** @} */

/*
 * Common Definitions
 */

void drizzle_library_init(void)
{
#ifdef USE_OPENSSL
  SSL_library_init();
#endif
#if defined(_WIN32)
  /* if it is MS windows, invoke WSAStartup */
  WSADATA wsaData;
  if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 )
  {
    printf("Error at WSAStartup()\n");
  }
#endif

}

void drizzle_library_deinit(void)
{
#if defined(_WIN32)
  /* if it is MS windows, invoke WSACleanup() at the end*/
  WSACleanup();
#endif
}

const char *drizzle_version(void)
{
  return LIBDRIZZLE_VERSION_STRING;
}

const char *drizzle_bugreport(void)
{
  return PACKAGE_BUGREPORT;
}

const char *drizzle_verbose_name(drizzle_verbose_t verbose)
{
  if (verbose >= DRIZZLE_VERBOSE_MAX)
  {
    return "UNKNOWN";
  }

  return _verbose_name[verbose];
}

int drizzle_timeout(const drizzle_st *con)
{
  if (con == NULL)
  {
    return -1;
  }

  return con->timeout;
}

void drizzle_set_timeout(drizzle_st *con, int timeout)
{
  if (con == NULL)
  {
    return;
  }

  con->timeout= timeout;
}

drizzle_verbose_t drizzle_verbose(const drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_VERBOSE_NEVER;
  }

  return con->verbose;
}

void drizzle_set_verbose(drizzle_st *con, drizzle_verbose_t verbose)
{
  if (con == NULL)
  {
    return;
  }

  con->verbose= verbose;
}

void drizzle_set_log_fn(drizzle_st *con, drizzle_log_fn *function,
                            void *context)
{
  if (con == NULL)
  {
    return;
  }

  con->log_fn= function;
  con->log_context= context;
}

drizzle_st *drizzle_create(void)
{
  drizzle_st *con;

  con= (drizzle_st*)malloc(sizeof(drizzle_st));
  if (con == NULL)
  {
    return NULL;
  }

  con->flags.is_shutdown= false;
  con->options= DRIZZLE_CON_NONE;
  con->packet_number= 0;
  con->protocol_version= 0;
  con->state_current= 0;
  con->events= 0;
  con->revents= 0;
  con->capabilities= DRIZZLE_CAPABILITIES_NONE;
  con->charset= DRIZZLE_CHARSET_NONE;
  con->command= DRIZZLE_COMMAND_SLEEP;
  con->socket_type= DRIZZLE_CON_SOCKET_TCP;
  con->status= DRIZZLE_CON_STATUS_NONE;
  con->max_packet_size= DRIZZLE_MAX_PACKET_SIZE;
  con->result_count= 0;
  con->thread_id= 0;
  con->backlog= DRIZZLE_DEFAULT_BACKLOG;
  con->fd= -1;
  con->buffer_size= 0;
  con->command_offset= 0;
  con->command_size= 0;
  con->command_total= 0;
  con->packet_size= 0;
  con->addrinfo_next= NULL;
  con->buffer_ptr= con->buffer;
  con->command_buffer= NULL;
  con->command_data= NULL;
  con->context= NULL;
  con->context_free_fn= NULL;
  con->result_list= NULL;
  con->scramble= NULL;
  con->socket.tcp.addrinfo= NULL;
  con->socket.tcp.host= NULL;
  con->socket.tcp.port= 0;
  /* con->buffer doesn't need to be set */
  con->db[0]= 0;
  con->password[0]= 0;
  /* con->scramble_buffer doesn't need to be set */
  con->server_version[0]= 0;
  /* con->state_stack doesn't need to be set */
  con->user[0]= 0;
  con->ssl_context= NULL;
  con->ssl= NULL;
  con->ssl_state= DRIZZLE_SSL_STATE_NONE;
  con->error_code= 0;
  con->verbose= DRIZZLE_VERBOSE_NEVER;
  con->last_errno= 0;
  con->timeout= -1;
  con->log_fn= NULL;
  con->log_context= NULL;
  con->sqlstate[0]= '\0';
  con->last_error[0]= '\0';

  return con;
}

void drizzle_free(drizzle_st *con)
{
  if (con == NULL)
  {
    return;
  }

  if (con->context != NULL && con->context_free_fn != NULL)
  {
    con->context_free_fn(con, con->context);
  }

  drizzle_result_free_all(con);

  if (con->fd != INVALID_SOCKET)
  {
    drizzle_close(con);
  }

  drizzle_reset_addrinfo(con);

#ifdef USE_OPENSSL
  if (con->ssl)
    SSL_free(con->ssl);

  if (con->ssl_context)
    SSL_CTX_free(con->ssl_context);
#endif

  free(con);
}

drizzle_return_t drizzle_wait(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (!con->events == 0)
  {
    con->pfds[0].fd= con->fd;
    con->pfds[0].events= con->events;
    con->pfds[0].revents= 0;
  }
  else
  {
    drizzle_set_error(con, __func__, "no active file descriptors");
    return DRIZZLE_RETURN_NO_ACTIVE_CONNECTIONS;
  }

  int ret;
  while (1)
  {
    drizzle_log_crazy(con, "poll timeout=%d", con->timeout);

    ret= poll(con->pfds, 1, con->timeout);

    drizzle_log_crazy(con, "poll return=%d errno=%d", ret, errno);

    if (ret == -1)
    {
      if (errno == EINTR)
      {
        continue;
      }

      drizzle_set_error(con, "drizzle_wait", "poll:%d", errno);
      con->last_errno= errno;
      return DRIZZLE_RETURN_ERRNO;
    }

    break;
  }

  if (ret == 0)
  {
    drizzle_set_error(con, "drizzle_wait", "timeout reached");
    return DRIZZLE_RETURN_TIMEOUT;
  }

  return drizzle_set_revents(con, con->pfds[0].revents);
}

drizzle_st *drizzle_ready(drizzle_st *con)
{
  /* We can't keep state between calls since connections may be removed during
     processing. If this list ever gets big, we may want something faster. */

  if (con->options & DRIZZLE_CON_IO_READY)
  {
    con->options = (drizzle_options_t)((int)con->options & (int)~DRIZZLE_CON_IO_READY);
    return con;
  }

  return NULL;
}

/*
 * Client Definitions
 */

drizzle_st *drizzle_create_tcp(const char *host, in_port_t port,
                               const char *user, const char *password,
                               const char *db,
                               drizzle_options_t options)
{
  drizzle_st *con= drizzle_create();
  if (con == NULL)
  {
    return NULL;
  }

  drizzle_set_tcp(con, host, port);
  drizzle_set_auth(con, user, password);
  drizzle_set_db(con, db);
  drizzle_add_options(con, options);

  return con;
}

drizzle_st *drizzle_create_uds(const char *uds, const char *user,
                                    const char *password, const char *db,
                                    drizzle_options_t options)
{
  drizzle_st *con;

  con= drizzle_create();
  if (con == NULL)
  {
    return NULL;
  }

  drizzle_set_uds(con, uds);
  drizzle_set_auth(con, user, password);
  drizzle_set_db(con, db);
  drizzle_add_options(con, options);

  return con;
}

/*
 * Local Definitions
 */

void drizzle_set_error(drizzle_st *con, const char *function,
                       const char *format, ...)
{
  if (con == NULL)
  {
    return;
  }

  size_t size;
  int written;
  char log_buffer[DRIZZLE_MAX_ERROR_SIZE];
  va_list args;

  size= strlen(function);
  char *ptr= (char *)memcpy(log_buffer, function, size);
  ptr+= size;
  ptr[0]= ':';
  size++;
  ptr++;

  va_start(args, format);
  written= vsnprintf(ptr, DRIZZLE_MAX_ERROR_SIZE - size, format, args);
  va_end(args);

  if (written < 0)
  {
    size= DRIZZLE_MAX_ERROR_SIZE;
  }
  else
  {
    size+= written;
  }

  if (size >= DRIZZLE_MAX_ERROR_SIZE)
  {
    size= DRIZZLE_MAX_ERROR_SIZE - 1;
  }
  log_buffer[size]= 0;

  if (con->log_fn == NULL)
  {
    memcpy(con->last_error, log_buffer, size + 1);
  }
  else
  {
    con->log_fn(log_buffer, DRIZZLE_VERBOSE_ERROR, con->log_context);
  }
}

void drizzle_log(drizzle_st *con, drizzle_verbose_t verbose,
                 const char *format, va_list args)
{
  if (con == NULL)
  {
    return;
  }

  char log_buffer[DRIZZLE_MAX_ERROR_SIZE];

  if (con->log_fn == NULL)
  {
    printf("%5s: ", drizzle_verbose_name(verbose));
    vprintf(format, args);
    printf("\n");
  }
  else
  {
    vsnprintf(log_buffer, DRIZZLE_MAX_ERROR_SIZE, format, args);
    log_buffer[DRIZZLE_MAX_ERROR_SIZE-1]= 0;
    con->log_fn(log_buffer, verbose, con->log_context);
  }
}
