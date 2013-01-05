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
 * @brief Struct Definitions
 */

#pragma once

#include <sys/types.h>

#if defined(HAVE_OPENSSL_SSL_H)
# include <openssl/ssl.h>
#endif

#ifdef NI_MAXHOST
# define LIBDRIZZLE_NI_MAXHOST NI_MAXHOST
#else
# define LIBDRIZZLE_NI_MAXHOST 1025
#endif

#ifdef __cplusplus
# include <cstddef>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "libdrizzle/datetime.h"

#if defined _WIN32 || defined __CYGWIN__
typedef SOCKET socket_t;
#else
typedef int socket_t;
#endif

/**
 * @ingroup drizzle_command 
 * Commands for drizzle_command functions.
 */
enum drizzle_command_t
{
  DRIZZLE_COMMAND_SLEEP,               /* Not used currently. */
  DRIZZLE_COMMAND_QUIT,
  DRIZZLE_COMMAND_INIT_DB,
  DRIZZLE_COMMAND_QUERY,
  DRIZZLE_COMMAND_FIELD_LIST,          /* Deprecated. */
  DRIZZLE_COMMAND_CREATE_DB,           /* Deprecated. */
  DRIZZLE_COMMAND_DROP_DB,             /* Deprecated. */
  DRIZZLE_COMMAND_REFRESH,
  DRIZZLE_COMMAND_SHUTDOWN,
  DRIZZLE_COMMAND_STATISTICS,
  DRIZZLE_COMMAND_PROCESS_INFO,        /* Deprecated. */
  DRIZZLE_COMMAND_CONNECT,             /* Not used currently. */
  DRIZZLE_COMMAND_PROCESS_KILL,        /* Deprecated. */
  DRIZZLE_COMMAND_DEBUG,
  DRIZZLE_COMMAND_PING,
  DRIZZLE_COMMAND_TIME,                /* Not used currently. */
  DRIZZLE_COMMAND_DELAYED_INSERT,      /* Not used currently. */
  DRIZZLE_COMMAND_CHANGE_USER,
  DRIZZLE_COMMAND_BINLOG_DUMP,         /* Not used currently. */
  DRIZZLE_COMMAND_TABLE_DUMP,          /* Not used currently. */
  DRIZZLE_COMMAND_CONNECT_OUT,         /* Not used currently. */
  DRIZZLE_COMMAND_REGISTER_SLAVE,      /* Not used currently. */
  DRIZZLE_COMMAND_STMT_PREPARE,
  DRIZZLE_COMMAND_STMT_EXECUTE,
  DRIZZLE_COMMAND_STMT_SEND_LONG_DATA,
  DRIZZLE_COMMAND_STMT_CLOSE,
  DRIZZLE_COMMAND_STMT_RESET,
  DRIZZLE_COMMAND_SET_OPTION,          /* Not used currently. */
  DRIZZLE_COMMAND_STMT_FETCH,
  DRIZZLE_COMMAND_DAEMON,              /* Not used currently. */
  DRIZZLE_COMMAND_BINLOG_DUMP_GTID,
  DRIZZLE_COMMAND_END                  /* Not used currently. */
};

#ifndef __cplusplus
typedef enum drizzle_command_t drizzle_command_t;
#endif


/**
 * @ingroup drizzle_con
 * Socket types for drizzle_st.
 */
enum drizzle_socket_t
{
  DRIZZLE_CON_SOCKET_TCP,
  DRIZZLE_CON_SOCKET_UDS
};

/**
 * @ingroup drizzle_con
 */
struct drizzle_tcp_st
{
  in_port_t port;
  struct addrinfo *addrinfo;
  char *host;
  char host_buffer[LIBDRIZZLE_NI_MAXHOST];
};

/**
 * @ingroup drizzle_con
 */
struct drizzle_uds_st
{
  char path_buffer[LIBDRIZZLE_NI_MAXHOST];
};

/**
 * @ingroup drizzle_con
 */
struct drizzle_st
{
  struct {
    bool is_shutdown;
  } flags;
  uint8_t packet_number;
  uint8_t protocol_version;
  uint8_t state_current;
  short events;
  short revents;
  drizzle_capabilities_t capabilities;
  drizzle_charset_t charset;
  drizzle_command_t command;
  drizzle_options_t options;
  drizzle_socket_t socket_type;
  drizzle_status_t status;
  uint32_t max_packet_size;
  uint32_t result_count;
  uint32_t thread_id;
  int backlog;
  socket_t fd;
  size_t buffer_size;
  size_t command_offset;
  size_t command_size;
  size_t command_total;
  size_t packet_size;
  struct addrinfo *addrinfo_next;
  unsigned char *buffer_ptr;
  unsigned char *command_buffer;
  unsigned char *command_data;
  void *context;
  drizzle_context_free_fn *context_free_fn;
  drizzle_result_st *result;
  drizzle_result_st *result_list;
  unsigned char *scramble;
  union
  {
    drizzle_tcp_st tcp;
    drizzle_uds_st uds;
  } socket;
  unsigned char buffer[DRIZZLE_MAX_BUFFER_SIZE];
  char db[DRIZZLE_MAX_DB_SIZE];
  char password[DRIZZLE_MAX_PASSWORD_SIZE];
  unsigned char scramble_buffer[DRIZZLE_MAX_SCRAMBLE_SIZE];
  char server_version[DRIZZLE_MAX_SERVER_VERSION_SIZE];
  char server_extra[DRIZZLE_MAX_SERVER_EXTRA_SIZE];
  drizzle_state_fn *state_stack[DRIZZLE_STATE_STACK_SIZE];
  char user[DRIZZLE_MAX_USER_SIZE];
#ifdef USE_OPENSSL
  SSL_CTX *ssl_context;
  SSL *ssl;
#else
  void *ssl_context;
  void *ssl;
#endif
  drizzle_ssl_state_t ssl_state;
  uint16_t error_code;
  drizzle_verbose_t verbose;
  int last_errno;
  int timeout;
  drizzle_log_fn *log_fn;
  void *log_context;
  pollfd_t pfds[1];
  char sqlstate[DRIZZLE_MAX_SQLSTATE_SIZE + 1];
  char last_error[DRIZZLE_MAX_ERROR_SIZE];
  drizzle_stmt_st *stmt;
};

/**
 * @ingroup drizzle_result
 */
struct drizzle_result_st
{
  drizzle_st *con;
  drizzle_result_st *next;
  drizzle_result_st *prev;
  drizzle_result_options_t options;

  char info[DRIZZLE_MAX_INFO_SIZE];
  uint16_t error_code;
  char sqlstate[DRIZZLE_MAX_SQLSTATE_SIZE + 1];
  uint64_t insert_id;
  uint16_t warning_count;
  uint64_t affected_rows;

  uint16_t column_count;
  uint16_t column_current;
  drizzle_column_st *column_list;
  drizzle_column_st *column;
  drizzle_column_st *column_buffer;

  uint64_t row_count;
  uint64_t row_current;

  uint16_t field_current;
  size_t field_total;
  size_t field_offset;
  size_t field_size;
  drizzle_field_t field;
  drizzle_field_t field_buffer;

  uint64_t row_list_size;
  drizzle_row_t row;
  drizzle_row_t *row_list;
  size_t *field_sizes;
  size_t **field_sizes_list;
  drizzle_binlog_st *binlog_event;
  bool binlog_checksums;
  uint8_t **null_bitmap_list;
  uint8_t *null_bitmap;
  uint8_t null_bitmap_length;
  bool binary_rows;

#ifdef __cplusplus

  drizzle_result_st() :
    con(NULL),
    next(NULL),
    prev(NULL),
    options(DRIZZLE_RESULT_NONE),
    error_code(0),
    insert_id(0),
    warning_count(0),
    affected_rows(0),
    column_count(0),
    column_current(0),
    column_list(NULL),
    column(NULL),
    column_buffer(NULL),
    row_count(0),
    row_current(0),
    field_current(0),
    field_total(0),
    field_offset(0),
    field_size(0),
    field(),
    field_buffer(),
    row_list_size(0),
    row(),
    row_list(NULL),
    field_sizes(NULL),
    field_sizes_list(NULL),
    binlog_event(NULL),
    binlog_checksums(false),
    null_bitmap_list(NULL),
    null_bitmap(NULL),
    null_bitmap_length(0),
    binary_rows(false)
  {
    info[0]= 0;
    sqlstate[0]= 0;
  }

#endif
};

struct drizzle_binlog_st
{
  uint32_t timestamp;
  drizzle_binlog_event_types_t type;
  uint32_t server_id;
  uint32_t length;
  uint32_t next_pos;
  uint16_t flags;
  uint16_t extra_flags;
  uint32_t checksum;
  unsigned char *data;
  unsigned char *raw_data;
  uint32_t raw_length;
};

/**
 * @ingroup drizzle_column
 */
struct drizzle_column_st
{
  drizzle_result_st *result;
  drizzle_column_st *next;
  drizzle_column_st *prev;
  drizzle_column_options_t options;
  char catalog[DRIZZLE_MAX_CATALOG_SIZE];
  char db[DRIZZLE_MAX_DB_SIZE];
  char table[DRIZZLE_MAX_TABLE_SIZE];
  char orig_table[DRIZZLE_MAX_TABLE_SIZE];
  char name[DRIZZLE_MAX_COLUMN_NAME_SIZE];
  char orig_name[DRIZZLE_MAX_COLUMN_NAME_SIZE];
  drizzle_charset_t charset;
  uint32_t size;
  size_t max_size;
  drizzle_column_type_t type;
  int flags;
  uint8_t decimals;
  unsigned char default_value[DRIZZLE_MAX_DEFAULT_VALUE_SIZE];
  size_t default_value_size;
};

struct drizzle_stmt_st
{
  drizzle_st *con;
  drizzle_stmt_state_t state;
  uint32_t id;
  uint16_t param_count;
  drizzle_bind_st *query_params;
  drizzle_bind_st *result_params;
  uint8_t null_bitmap_length;
  uint8_t *null_bitmap;
  bool new_bind;
  drizzle_result_st *prepare_result;
  drizzle_result_st *execute_result;
  drizzle_column_st *fields;
};

struct drizzle_bind_st
{
  drizzle_column_type_t type;
  void *data;
  uint32_t length;
  bool is_bound;
  char *converted_data;
  struct
  {
    bool is_null;
    bool is_unsigned;
    bool is_long_data;
    bool is_allocated;
  } options;
};

#ifdef __cplusplus
}
#endif
