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

#include "src/datetime.h"
#include "src/packet.h"

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

struct drizzle_options_st
{
  bool non_blocking;
  bool raw_scramble;
  bool found_rows;
  bool interactive;
  bool multi_statements;
  bool auth_plugin;
  drizzle_socket_owner_t socket_owner;
  int wait_timeout;
  int keepidle;  // default value under linux: 7200
  int keepcnt;   // default value under linux: 75
  int keepintvl; // default value under linux: 9

  drizzle_options_st() :
    non_blocking(false),
    raw_scramble(false),
    found_rows(false),
    interactive(false),
    multi_statements(false),
    auth_plugin(false),
    socket_owner(DRIZZLE_SOCKET_OWNER_NATIVE),
    wait_timeout(DRIZZLE_DEFAULT_SOCKET_TIMEOUT),
    keepidle(7200),
    keepcnt(75),
    keepintvl(9)
  { }
};

struct drizzle_st
{
  struct flags_t{
    bool is_shutdown;

    flags_t() :
      is_shutdown(false)
    { }
  } flags;
  uint8_t packet_number;
  uint8_t protocol_version;
  short events;
  short revents;
  drizzle_capabilities_t capabilities;
  drizzle_charset_t charset;
  drizzle_command_t command;
  struct state_t
  {
    bool ready;
    bool no_result_read;
    bool io_ready;
    bool raw_packet;

    state_t() :
      ready(false),
      no_result_read(false),
      io_ready(false),
      raw_packet(false)
    { }
  } state;

  drizzle_options_st options;
  drizzle_socket_t socket_type;
  drizzle_status_t status;
  uint32_t max_packet_size;
  uint32_t result_count;
  uint32_t thread_id;
  int backlog;
  socket_t fd;
  size_t buffer_size;              /* amount of valid data after 'buffer_ptr' in 'buffer' */
  size_t command_offset;
  size_t command_size;
  size_t command_total;
  uint32_t packet_size;            /* remaining number of bytes in packet currently being read; maximum value 2^24 */
  struct addrinfo *addrinfo_next;
  unsigned char *buffer_ptr;       /* cursor pointing into 'buffer' */
  unsigned char *command_buffer;
  unsigned char *command_data;
  void *context;
  drizzle_context_free_fn *context_free_fn;
  void *event_watch_context; /* context for custom callback function  */
  drizzle_event_watch_fn *event_watch_fn; /* custom call back function */
  drizzle_result_st *result;
  drizzle_result_st *result_list;
  unsigned char *scramble;
  union
  {
    drizzle_tcp_st tcp;
    drizzle_uds_st uds;
  } socket;
  unsigned char *buffer;
  size_t buffer_allocation; /* total allocated size of 'buffer' */
  char db[DRIZZLE_MAX_DB_SIZE];
  char password[DRIZZLE_MAX_PASSWORD_SIZE + 1];
  unsigned char scramble_buffer[DRIZZLE_MAX_SCRAMBLE_SIZE];
  char server_version[DRIZZLE_MAX_SERVER_VERSION_SIZE];
  char server_extra[DRIZZLE_MAX_SERVER_EXTRA_SIZE];
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
  drizzle_binlog_st *binlog;
private:
  size_t _state_stack_count;
  Packet *_state_stack_list;
  Packet _stack_packets[DRIZZLE_STATE_STACK_SIZE];
  size_t _free_packet_count;
  Packet *_free_packet_list;
public:

  drizzle_st() :
    packet_number(0),
    protocol_version(0),
    events(0),
    revents(0),
    capabilities(DRIZZLE_CAPABILITIES_NONE),
    charset(DRIZZLE_CHARSET_NONE),
    command(DRIZZLE_COMMAND_SLEEP),
    socket_type(DRIZZLE_CON_SOCKET_TCP),
    status(DRIZZLE_CON_STATUS_NONE),
    max_packet_size(DRIZZLE_MAX_PACKET_SIZE),
    result_count(0),
    thread_id(0),
    backlog(DRIZZLE_DEFAULT_BACKLOG),
    fd(-1),
    buffer_size(0),
    command_offset(0),
    command_size(0),
    command_total(0),
    packet_size(0),
    addrinfo_next(NULL),
    command_buffer(NULL),
    command_data(NULL),
    context(NULL),
    context_free_fn(NULL),
    event_watch_context(NULL),
    event_watch_fn(NULL),
    result(NULL),
    result_list(NULL),
    scramble(NULL),
    buffer_allocation(DRIZZLE_DEFAULT_BUFFER_SIZE),
    ssl_context(NULL),
    ssl(NULL),
    ssl_state(DRIZZLE_SSL_STATE_NONE),
    error_code(0),
    verbose(DRIZZLE_VERBOSE_NEVER),
    last_errno(0),
    timeout(-1),
    log_fn(NULL),
    log_context(NULL),
    stmt(NULL),
    binlog(NULL),
    _state_stack_count(0),
    _state_stack_list(NULL),
    _free_packet_count(0),
    _free_packet_list(NULL)
  {
    db[0]= '\0';
    password[0]= '\0';
    server_version[0]= '\0';
    user[0]= '\0';
    sqlstate[0]= '\0';
    last_error[0]= '\0';
    buffer= (unsigned char*)malloc(DRIZZLE_DEFAULT_BUFFER_SIZE);
    buffer_ptr= buffer;

    assert(DRIZZLE_STATE_STACK_SIZE);
    for (size_t x= 0; x < DRIZZLE_STATE_STACK_SIZE; ++x)
    {
      Packet *packet= &(_stack_packets[x]);
      packet->init(this);
      LIBDRIZZLE_LIST_ADD(_free_packet, packet);
      assert(_free_packet_list);
    }
  }

  ~drizzle_st()
  {
    clear_state();
  }

  bool push_state(drizzle_state_fn* func_)
  {
    Packet *tmp;
    if (_free_packet_count)
    {
      tmp= _free_packet_list;
      LIBDRIZZLE_LIST_DEL(_free_packet, tmp);
      tmp->func(func_);
    }
    else
    {
      tmp= new (std::nothrow) Packet(this, func_);
    }

    if (tmp)
    {
      LIBDRIZZLE_LIST_ADD(_state_stack, tmp);
      return true;
    }

    return false;
  }

  bool has_state() const
  {
    return _state_stack_count == 0;
  }

  drizzle_return_t current_state()
  {
    return _state_stack_list->func();
  }

  void pop_state()
  {
    Packet* tmp= _state_stack_list;
    if (tmp)
    {
      LIBDRIZZLE_LIST_DEL(_state_stack, tmp);

      if (tmp->stack())
      {
        tmp->clear();
        LIBDRIZZLE_LIST_ADD(_free_packet, tmp);
      }
      else
      {
        delete tmp;
      }
    }
  }

  void clear_state()
  {
    while(_state_stack_list)
    {
      pop_state();
    }
  }
};

struct drizzle_binlog_event_st
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
  drizzle_binlog_event_st() :
    timestamp(0),
    type(DRIZZLE_EVENT_TYPE_UNKNOWN),
    server_id(0),
    length(0),
    next_pos(0),
    flags(0),
    extra_flags(0),
    checksum(0),
    data(NULL),
    raw_data(NULL),
    raw_length(0)
  { }
};

struct drizzle_binlog_st
{
  drizzle_binlog_fn *binlog_fn;
  drizzle_binlog_error_fn *error_fn;
  void *binlog_context;
  drizzle_binlog_event_st event;
  bool verify_checksums;
  bool has_checksums;
  drizzle_st *con;
  drizzle_binlog_st() :
    binlog_fn(NULL),
    error_fn(NULL),
    binlog_context(NULL),
    verify_checksums(false),
    has_checksums(false),
    con(NULL)
  { }
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

  drizzle_column_st() :
    result(NULL),
    next(NULL),
    prev(NULL),
    options(DRIZZLE_COLUMN_UNUSED),
    charset(DRIZZLE_CHARSET_NONE),
    size(0),
    max_size(0),
    type(DRIZZLE_COLUMN_TYPE_NONE),
    flags(DRIZZLE_COLUMN_FLAGS_NONE),
    decimals(0),
    default_value_size(0)
  {
    catalog[0]= '\0';
    db[0]= '\0';
    table[0]= '\0';
    orig_table[0] ='\0';
    name[0]= '\0';
  }
};

struct drizzle_stmt_st
{
  drizzle_st *con;
  drizzle_stmt_state_t state;
  uint32_t id;
  uint16_t param_count;
  drizzle_bind_st *query_params;
  drizzle_bind_st *result_params;
  uint16_t null_bitmap_length;
  uint8_t *null_bitmap;
  bool new_bind;
  drizzle_result_st *prepare_result;
  drizzle_result_st *execute_result;
  drizzle_column_st *fields;

  drizzle_stmt_st() :
    con(NULL),
    state(DRIZZLE_STMT_NONE),
    id(0),
    param_count(0),
    query_params(NULL),
    result_params(NULL),
    null_bitmap_length(0),
    null_bitmap(NULL),
    new_bind(true),
    prepare_result(NULL),
    execute_result(NULL),
    fields(NULL)
  { }
};

struct drizzle_bind_st
{
  drizzle_column_type_t type;
  void *data;
  char *data_buffer;
  size_t length;  /* amount of data in 'data' */
  bool is_bound;
  struct options_t
  {
    bool is_null;
    bool is_unsigned;
    bool is_long_data;

    options_t() :
      is_null(false),
      is_unsigned(false),
      is_long_data(false)
    { }
  } options;
  drizzle_bind_st() :
    type(DRIZZLE_COLUMN_TYPE_NONE),
    data(NULL),
    length(0),
    is_bound(false)
  {
    data_buffer= new (std::nothrow) char[128];
  }
};

#ifdef __cplusplus
}
#endif
