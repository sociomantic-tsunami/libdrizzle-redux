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
 * @brief Handshake Definitions
 */

#include "config.h"
#include "src/common.h"
#ifdef USE_OPENSSL
#include <openssl/err.h>
#endif

/*
 * Client Definitions
 */

drizzle_return_t drizzle_handshake_server_read(drizzle_st *con)
{
  if (con->has_state())
  {
    con->push_state(drizzle_state_handshake_server_read);
    con->push_state(drizzle_state_packet_read);
  }

  return drizzle_state_loop(con);
}

drizzle_return_t drizzle_handshake_client_write(drizzle_st *con)
{
  if (con->has_state())
  {
    con->push_state(drizzle_state_write);
    con->push_state(drizzle_state_handshake_client_write);

    if (con->ssl)
    {
      con->push_state(drizzle_state_write);
      con->push_state(drizzle_state_handshake_ssl_client_write);
    }
  }

  return drizzle_state_loop(con);
}

/*
 * State Definitions
 */

drizzle_return_t drizzle_state_handshake_server_read(drizzle_st *con)
{
  unsigned char *ptr;
  ptrdiff_t extra_length;
  unsigned char* packet_end;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  __LOG_LOCATION__

  /* Assume the entire handshake packet will fit in the buffer. */
  if (con->buffer_size < con->packet_size)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (drizzle_check_unpack_error(con))
  {
    return DRIZZLE_RETURN_ERROR_CODE;
  }
  else if (con->packet_size < 46)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "bad packet size:>=46:%" PRIu32, con->packet_size);
    return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
  }

  packet_end= con->buffer_ptr + con->packet_size;
  con->protocol_version= con->buffer_ptr[0];
  con->buffer_ptr++;

  if (con->protocol_version != 10)
  {
    /* This is a special case where the server determines that authentication
       will be impossible and denies any attempt right away. */
    if (con->protocol_version == 255)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__,
                        "%.*s", (int)con->packet_size - 3,
                        con->buffer_ptr + 2);
      return DRIZZLE_RETURN_AUTH_FAILED;
    }

    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "protocol version not supported:%d",
                      con->protocol_version);
    return DRIZZLE_RETURN_PROTOCOL_NOT_SUPPORTED;
  }

  /* Look for null-terminated server version string. */
  ptr= (unsigned char*)memchr(con->buffer_ptr, 0, con->buffer_size - 1);
  if (ptr == NULL)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "server version string not found");
    return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
  }

  if (con->packet_size < (46 + (size_t)(ptr - con->buffer_ptr)))
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "bad packet size:%" PRIu64 ":%" PRIu32,
                      (46 + (size_t)(ptr - con->buffer_ptr)), con->packet_size);
    return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
  }

  strncpy(con->server_version, (char *)con->buffer_ptr,
          DRIZZLE_MAX_SERVER_VERSION_SIZE);
  con->server_version[DRIZZLE_MAX_SERVER_VERSION_SIZE - 1]= 0;
  con->buffer_ptr+= ((ptr - con->buffer_ptr) + 1);

  con->thread_id= (uint32_t)drizzle_get_byte4(con->buffer_ptr);
  con->buffer_ptr+= 4;

  con->scramble= con->scramble_buffer;
  memcpy(con->scramble, con->buffer_ptr, 8);
  /* Skip scramble and filler. */
  con->buffer_ptr+= 9;

  /* Even though drizzle_capabilities is more than 2 bytes, the protocol only
     allows for 2. This means some capabilities are not possible during this
     handshake step. The options beyond 2 bytes are for client response only. */
  con->capabilities= (drizzle_capabilities_t)drizzle_get_byte2(con->buffer_ptr);
  con->buffer_ptr+= 2;

  if (!(con->capabilities & DRIZZLE_CAPABILITIES_PROTOCOL_41))
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "protocol version not supported, must be MySQL 4.1+");
    return DRIZZLE_RETURN_PROTOCOL_NOT_SUPPORTED;
  }

  con->charset= (drizzle_charset_t)con->buffer_ptr[0];
  con->buffer_ptr+= 1;

  con->status= (drizzle_status_t)drizzle_get_byte2(con->buffer_ptr);
  /* Skip status and filler. */
  con->buffer_ptr+= 15;

  memcpy(con->scramble + 8, con->buffer_ptr, 12);
  con->buffer_ptr+= 13;

  /* MySQL 5.5 adds "mysql_native_password" after the server greeting. */
  extra_length= packet_end - con->buffer_ptr;
  assert(extra_length >= 0);
  if (extra_length > DRIZZLE_MAX_SERVER_EXTRA_SIZE - 1)
    extra_length= DRIZZLE_MAX_SERVER_EXTRA_SIZE - 1;
  memcpy(con->server_extra, (char *)con->buffer_ptr, extra_length);
  con->server_extra[extra_length]= 0;

  con->buffer_size-= con->packet_size;
  if (con->buffer_size != 0)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "unexpected data after packet:%" PRIu64, con->buffer_size);
    return DRIZZLE_RETURN_UNEXPECTED_DATA;
  }

  con->buffer_ptr= con->buffer;

  con->pop_state();

  if (con->state.raw_packet == false)
  {
    con->push_state(drizzle_state_handshake_result_read);
    con->push_state(drizzle_state_packet_read);
    con->push_state(drizzle_state_write);
    con->push_state(drizzle_state_handshake_client_write);
    if (con->ssl)
    {
      con->push_state(drizzle_state_write);
      con->push_state(drizzle_state_handshake_ssl_client_write);
    }
  }

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_handshake_server_write(drizzle_st *con)
{
  unsigned char *ptr;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  __LOG_LOCATION__

  /* Calculate max packet size. */
  con->packet_size= (uint32_t)(
                    1   /* Protocol version */
                  + strlen(con->server_version) + 1
                  + 4   /* Thread ID */
                  + 8   /* Scramble */
                  + 1   /* NULL */
                  + 2   /* Capabilities */
                  + 1   /* Language */
                  + 2   /* Status */
                  + 13  /* Unused */
                  + 12  /* Scramble */
                  + 1);  /* NULL */

  /* Assume the entire handshake packet will fit in the buffer. */
  if ((con->packet_size + 4) > con->buffer_allocation)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "buffer too small:%" PRIu32, con->packet_size + 4);
    return DRIZZLE_RETURN_INTERNAL_ERROR;
  }

  ptr= con->buffer_ptr;

  /* Store packet size and packet number first. */
  drizzle_set_byte3(ptr, con->packet_size);
  ptr[3]= 0;
  con->packet_number= 1;
  ptr+= 4;

  ptr[0]= con->protocol_version;
  ptr++;

  memcpy(ptr, con->server_version, strlen(con->server_version));
  ptr+= strlen(con->server_version);

  ptr[0]= 0;
  ptr++;

  drizzle_set_byte4(ptr, con->thread_id);
  ptr+= 4;

  if (con->scramble == NULL)
  {
    memset(ptr, 0, 8);
  }
  else
  {
    memcpy(ptr, con->scramble, 8);
  }
  ptr+= 8;

  ptr[0]= 0;
  ptr++;

  con->capabilities = (drizzle_capabilities_t)((int) con->capabilities | (int)DRIZZLE_CAPABILITIES_PROTOCOL_41);

  /* We can only send two bytes worth, this is a protocol limitation. */
  drizzle_set_byte2(ptr, con->capabilities);
  ptr+= 2;

  ptr[0]= con->charset;
  ptr++;

  drizzle_set_byte2(ptr, con->status);
  ptr+= 2;

  memset(ptr, 0, 13);
  ptr+= 13;

  if (con->scramble == NULL)
  {
    memset(ptr, 0, 12);
  }
  else
  {
    memcpy(ptr, con->scramble + 8, 12);
  }
  ptr+= 12;

  ptr[0]= 0;
  ptr++;

  con->buffer_size+= (4 + con->packet_size);

  /* Make sure we packed it correctly. */
  if ((size_t)(ptr - con->buffer_ptr) != (4 + con->packet_size))
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "error packing server handshake:%" PRIu64 ":%" PRIu32,
                      (size_t)(ptr - con->buffer_ptr), 4 + con->packet_size);
    return DRIZZLE_RETURN_INTERNAL_ERROR;
  }

  con->pop_state();
  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_handshake_client_read(drizzle_st *con)
{
  size_t real_size;
  uint8_t scramble_size;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  __LOG_LOCATION__

  /* Assume the entire handshake packet will fit in the buffer. */
  if (con->buffer_size < con->packet_size)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  /* This is the minimum packet size. */
  if (con->packet_size < 34)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "bad packet size:>=34:%" PRIu32, con->packet_size);
    return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
  }

  real_size= 34;

  con->capabilities= (drizzle_capabilities_t) drizzle_get_byte4(con->buffer_ptr);
  con->buffer_ptr+= 4;

  if (!(con->capabilities & DRIZZLE_CAPABILITIES_PROTOCOL_41))
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "protocol version not supported, must be MySQL 4.1+");
    return DRIZZLE_RETURN_PROTOCOL_NOT_SUPPORTED;
  }

  con->max_packet_size= (uint32_t)drizzle_get_byte4(con->buffer_ptr);
  con->buffer_ptr+= 4;

  con->charset= (drizzle_charset_t)con->buffer_ptr[0];
  con->buffer_ptr+= 1;

  /* Skip unused. */
  con->buffer_ptr+= 23;

  /* Look for null-terminated user string. */
  unsigned char *ptr= (unsigned char*)memchr(con->buffer_ptr, 0, con->buffer_size - 32);
  if (ptr == NULL)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "user string not found");
    return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
  }

  if (con->buffer_ptr == ptr)
  {
    con->user[0]= 0;
    con->buffer_ptr++;
  }
  else
  {
    real_size+= (size_t)(ptr - con->buffer_ptr);
    if (con->packet_size < real_size)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__,
                        "bad packet size:>=%" PRIu64 ":%" PRIu32, real_size,
                        con->packet_size);
      return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
    }

    strncpy(con->user, (char *)con->buffer_ptr, DRIZZLE_MAX_USER_SIZE);
    con->user[DRIZZLE_MAX_USER_SIZE - 1]= 0;
    con->buffer_ptr+= ((ptr - con->buffer_ptr) + 1);
  }

  scramble_size= con->buffer_ptr[0];
  con->buffer_ptr+= 1;

  if (scramble_size == 0)
  {
    con->scramble= NULL;
  }
  else
  {
    if (scramble_size != DRIZZLE_MAX_SCRAMBLE_SIZE)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__,
                        "wrong scramble size");
      return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
    }

    real_size+= scramble_size;
    con->scramble= con->scramble_buffer;
    memcpy(con->scramble, con->buffer_ptr, DRIZZLE_MAX_SCRAMBLE_SIZE);

    con->buffer_ptr+= DRIZZLE_MAX_SCRAMBLE_SIZE;
  }

  /* Look for null-terminated db string. */
  if ((34 + strlen(con->user) + scramble_size) == con->packet_size)
  {
    con->db[0]= 0;
  }
  else
  {
    ptr= (unsigned char*)memchr(con->buffer_ptr, 0, con->buffer_size -
                                    (34 + strlen(con->user) + scramble_size));
    if (ptr == NULL)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__,
                        "db string not found");
      return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
    }

    real_size+= ((size_t)(ptr - con->buffer_ptr) + 1);
    if (con->packet_size != real_size)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__,
                        "bad packet size:%" PRIu64 ":%" PRIu64,
                        real_size, con->packet_size);
      return DRIZZLE_RETURN_BAD_HANDSHAKE_PACKET;
    }

    if (con->buffer_ptr == ptr)
    {
      con->db[0]= 0;
      con->buffer_ptr++;
    }
    else
    {
      strncpy(con->db, (char *)con->buffer_ptr, DRIZZLE_MAX_DB_SIZE);
      con->db[DRIZZLE_MAX_DB_SIZE - 1]= 0;
      con->buffer_ptr+= ((ptr - con->buffer_ptr) + 1);
    }
  }

  con->buffer_size-= con->packet_size;
  if (con->buffer_size != 0)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "unexpected data after packet:%" PRIu64, con->buffer_size);
    return DRIZZLE_RETURN_UNEXPECTED_DATA;
  }

  con->buffer_ptr= con->buffer;

  con->pop_state();
  return DRIZZLE_RETURN_OK;
}

int drizzle_compile_capabilities(drizzle_st *con)
{
  int capabilities;

  con->capabilities = (drizzle_capabilities_t)((int)con->capabilities | (int)DRIZZLE_CAPABILITIES_PROTOCOL_41);

  capabilities= con->capabilities & DRIZZLE_CAPABILITIES_CLIENT;
  if (con->options.found_rows == false)
  {
    capabilities&= ~DRIZZLE_CAPABILITIES_FOUND_ROWS;
  }

  if (con->options.interactive)
  {
    capabilities|= DRIZZLE_CAPABILITIES_INTERACTIVE;
  }

  if (con->options.multi_statements)
  {
    capabilities|= DRIZZLE_CAPABILITIES_MULTI_STATEMENTS;
  }

  if (con->options.auth_plugin)
  {
    capabilities|= DRIZZLE_CAPABILITIES_PLUGIN_AUTH;
  }
#ifdef USE_OPENSSL
  if (con->ssl)
  {
    capabilities|= DRIZZLE_CAPABILITIES_SSL;
  }
#endif
  capabilities&= ~DRIZZLE_CAPABILITIES_COMPRESS;
  if (con->db[0] == 0)
    capabilities&= ~DRIZZLE_CAPABILITIES_CONNECT_WITH_DB;

  return capabilities;
}

drizzle_return_t drizzle_state_handshake_client_write(drizzle_st *con)
{
  unsigned char *ptr;
  int capabilities;
#ifdef USE_OPENSSL
  int ssl_ret;
#endif
  drizzle_return_t ret;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  __LOG_LOCATION__
#ifdef USE_OPENSSL
  if (con->ssl)
  {
    ERR_clear_error();
    ssl_ret= SSL_connect(con->ssl);
    if (0 == ssl_ret) {
            // it was shut down
            ERR_clear_error();
            SSL_shutdown(con->ssl);
            return DRIZZLE_RETURN_LOST_CONNECTION;
    }
    if (ssl_ret < 0) {
            int rc = SSL_get_error(con->ssl, ssl_ret);
            drizzle_return_t rsev;
            switch (rc) {
            case SSL_ERROR_WANT_READ:
                    con->revents = 0;
                    rsev = drizzle_set_events(con, POLLIN);
                    return DRIZZLE_RETURN_OK == rsev ?
                            DRIZZLE_RETURN_IO_WAIT : rsev;
            case SSL_ERROR_WANT_WRITE:
                    con->revents = 0;
                    rsev = drizzle_set_events(con, POLLOUT);
                    return DRIZZLE_RETURN_OK == rsev ?
                            DRIZZLE_RETURN_IO_WAIT : rsev;
            case SSL_ERROR_ZERO_RETURN:
                    // peer closed connection. Isn't this case handled by
                    // ssl_ret == 0?
                    ERR_clear_error();
                    SSL_shutdown(con->ssl);
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
    con->ssl_state= DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE;
  }
#endif
  /* Calculate max packet size. */
  con->packet_size= (uint32_t)(
                    4   /* Capabilities */
                  + 4   /* Max packet size */
                  + 1   /* Charset */
                  + 23  /* Unused */
                  + strlen(con->user) + 1
                  + 1   /* Scramble size */
                  + DRIZZLE_MAX_SCRAMBLE_SIZE
                  + strlen(con->db) + 1);

  /* Assume the entire handshake packet will fit in the buffer. */
  if ((con->packet_size + 4) > con->buffer_allocation)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "buffer too small:%" PRIu64, con->packet_size + 4);
    return DRIZZLE_RETURN_INTERNAL_ERROR;
  }

  ptr= con->buffer_ptr;

  /* Store packet size at the end since it may change. */
  ptr[3]= con->packet_number;
  con->packet_number++;
  ptr+= 4;

  capabilities= drizzle_compile_capabilities(con);

  drizzle_set_byte4(ptr, capabilities);
  ptr+= 4;

  drizzle_set_byte4(ptr, con->max_packet_size);
  ptr+= 4;

  ptr[0]= con->charset;
  ptr++;

  memset(ptr, 0, 23);
  ptr+= 23;

  ptr= drizzle_pack_auth(con, ptr, &ret);
  if (ret != DRIZZLE_RETURN_OK)
    return ret;

  con->buffer_size+= (4 + con->packet_size);

  /* Make sure we packed it correctly. */
  if ((size_t)(ptr - con->buffer_ptr) != (4 + con->packet_size))
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__,
                      "error packing client handshake:" PRIu64 ":%" PRIu32,
                      (size_t)(ptr - con->buffer_ptr), 4 + con->packet_size);
    return DRIZZLE_RETURN_INTERNAL_ERROR;
  }

  /* Store packet size now. */
  drizzle_set_byte3(con->buffer_ptr, con->packet_size);

  con->pop_state();
  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_handshake_ssl_client_write(drizzle_st *con)
{
  unsigned char *ptr;
  int capabilities;

  __LOG_LOCATION__

  /* SSL handshake packet structure */
  con->packet_size= 4   /* Capabilities */
                  + 4   /* Max packet size */
                  + 1   /* Charset */
                  + 23; /* Padding unused bytes */

  ptr= con->buffer_ptr;
  drizzle_set_byte3(ptr, con->packet_size);
  ptr[3]= con->packet_number;
  con->packet_number++;
  ptr+= 4;

  capabilities= drizzle_compile_capabilities(con);
  drizzle_set_byte4(ptr, capabilities);
  ptr+= 4;
  drizzle_set_byte4(ptr, con->max_packet_size);
  ptr+= 4;

  ptr[0]= con->charset;

  con->buffer_size+= con->packet_size + 4;
  ptr++;

  memset(ptr, 0, 23);

  con->pop_state();
  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_handshake_result_read(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  __LOG_LOCATION__

  drizzle_result_st *result = drizzle_result_create(con);

  if (result == NULL)
  {
    return DRIZZLE_RETURN_MEMORY;
  }

  con->result= result;

  drizzle_return_t ret= drizzle_state_result_read(con);
  if (con->has_state())
  {
    if (ret == DRIZZLE_RETURN_OK)
    {
      if (drizzle_result_eof(result))
      {
        drizzle_set_error(con, __FILE_LINE_FUNC__,
                         "old insecure authentication mechanism not supported");
        ret= DRIZZLE_RETURN_AUTH_FAILED;
      }
      else
      {
        con->state.ready= true;
      }
    }
  }

  drizzle_result_free(result);

  if (ret == DRIZZLE_RETURN_ERROR_CODE)
  {
    return DRIZZLE_RETURN_HANDSHAKE_FAILED;
  }

  return ret;
}
