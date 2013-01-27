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
 * @brief Drizzle Definitions
 */

#include "config.h"
#include "libdrizzle/common.h"

#include <cerrno>
#include <pthread.h>

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

static pthread_once_t ssl_startup_once= PTHREAD_ONCE_INIT;

static void drizzle_library_deinit(void)
{ 
#if defined(_WIN32)
  /* if it is MS windows, invoke WSACleanup() at the end*/
  WSACleanup();
#endif
}

static void ssl_startup_function(void)
{
#ifdef USE_OPENSSL
  SSL_library_init();
  ERR_load_crypto_strings();
  SSL_load_error_strings();
  OpenSSL_add_all_algorithms();
#endif
#if defined(_WIN32)
  /* if it is MS windows, invoke WSAStartup */
  WSADATA wsaData;
  if ( WSAStartup( MAKEWORD(2,2), &wsaData ) != 0 )
  {
    fprintf(stderr, "Error at WSAStartup()\n");
  }
#endif

  (void)atexit(drizzle_library_deinit);
}

bool drizzle_library_init(drizzle_st* connection)
{
  int pthread_error;
  if ((pthread_error= pthread_once(&ssl_startup_once, ssl_startup_function)) == -1)
  {
    drizzle_set_error(connection, "pthread_once", "error:%s", strerror(errno));
    return false;
  }

  return true;
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

void drizzle_set_log_fn(drizzle_st *con, drizzle_log_fn *function, void *context)
{
  if (con == NULL)
  {
    return;
  }

  con->log_fn= function;
  con->log_context= context;
}

drizzle_st *drizzle_clone(drizzle_st *drizzle, const drizzle_st *from)
{
  drizzle= new (std::nothrow) drizzle_st;
  if (drizzle == NULL)
  {
    return NULL;
  }

  drizzle->capabilities= from->capabilities;
  drizzle->options= from->options;

  drizzle->backlog= from->backlog;
  strcpy(drizzle->db, from->db);
  strcpy(drizzle->password, from->password);
  strcpy(drizzle->user, from->user);

  switch (from->socket_type)
  {
  case DRIZZLE_CON_SOCKET_TCP:
    drizzle_set_tcp(drizzle, from->socket.tcp.host, from->socket.tcp.port);
    break;

  case DRIZZLE_CON_SOCKET_UDS:
    drizzle_set_uds(drizzle, from->socket.uds.path_buffer);
    break;

  default:
    break;
  }

  return drizzle;
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

  if (con->binlog != NULL)
  {
    drizzle_binlog_free(con->binlog);
  }

  free(con->buffer);
  delete con;
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
    drizzle_log_debug(con, "poll timeout=%d", con->timeout);

    ret= poll(con->pfds, 1, con->timeout);

    drizzle_log_debug(con, "poll return=%d errno=%d", ret, errno);

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

  if (con->state.io_ready)
  {
    con->state.io_ready= false;
    return con;
  }

  return NULL;
}

/*
 * Client Definitions
 */

drizzle_st *drizzle_create(const char *host, in_port_t port,
                           const char *user, const char *password,
                           const char *db,
                           drizzle_options_st *options)
{
  drizzle_st *con= new (std::nothrow) drizzle_st;
  if (con == NULL)
  {
    return NULL;
  }

  if (drizzle_library_init(con) == false)
  {
    delete con;
    return NULL;
  }

  if (host and host[0] == '/')
  {
    drizzle_set_uds(con, host);
  }
  else
  {
    drizzle_set_tcp(con, host, port);
  }
  drizzle_set_auth(con, user, password);
  drizzle_set_db(con, db);
  if (options != NULL)
  {
    con->options= *options;
  }

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
