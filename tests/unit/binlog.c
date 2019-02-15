/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012-2013 Drizzle Developer Group
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

#include <yatl/lite.h>

#include <libdrizzle-redux/libdrizzle.h>

#include "tests/unit/common.h"

struct binlog_context
{
  uint32_t next_pos;
};
typedef struct binlog_context binlog_context;

const drizzle_binlog_event_types_t binlog_event_types[] =
  {
    DRIZZLE_EVENT_TYPE_UNKNOWN,
    DRIZZLE_EVENT_TYPE_START,
    DRIZZLE_EVENT_TYPE_QUERY,
    DRIZZLE_EVENT_TYPE_STOP,
    DRIZZLE_EVENT_TYPE_ROTATE,
    DRIZZLE_EVENT_TYPE_INTVAR,
    DRIZZLE_EVENT_TYPE_LOAD,
    DRIZZLE_EVENT_TYPE_SLAVE,
    DRIZZLE_EVENT_TYPE_CREATE_FILE,
    DRIZZLE_EVENT_TYPE_APPEND_BLOCK,
    DRIZZLE_EVENT_TYPE_EXEC_LOAD,
    DRIZZLE_EVENT_TYPE_DELETE_FILE,
    DRIZZLE_EVENT_TYPE_NEW_LOAD,
    DRIZZLE_EVENT_TYPE_RAND,
    DRIZZLE_EVENT_TYPE_USER_VAR,
    DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION,
    DRIZZLE_EVENT_TYPE_XID,
    DRIZZLE_EVENT_TYPE_BEGIN_LOAD_QUERY,
    DRIZZLE_EVENT_TYPE_EXECUTE_LOAD_QUERY,
    DRIZZLE_EVENT_TYPE_TABLE_MAP,
    // Next 3 are used in 5.1.0 -> 5.1.15 only
    DRIZZLE_EVENT_TYPE_OBSOLETE_WRITE_ROWS,
    DRIZZLE_EVENT_TYPE_OBSOLETE_UPDATE_ROWS,
    DRIZZLE_EVENT_TYPE_OBSOLETE_DELETE_ROWS,
    DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS,
    DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS,
    DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS,
    DRIZZLE_EVENT_TYPE_INCIDENT,
    DRIZZLE_EVENT_TYPE_HEARTBEAT,
    DRIZZLE_EVENT_TYPE_IGNORABLE,
    DRIZZLE_EVENT_TYPE_ROWS_QUERY,
    DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS,
    DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS,
    DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS,
    DRIZZLE_EVENT_TYPE_GTID,
    DRIZZLE_EVENT_TYPE_ANONYMOUS_GTID,
    DRIZZLE_EVENT_TYPE_PREVIOUS_GTIDS,
    DRIZZLE_EVENT_TYPE_END
  };

const char *binlog_event_types_name[] =
  {
    "DRIZZLE_EVENT_TYPE_UNKNOWN",
    "DRIZZLE_EVENT_TYPE_START",
    "DRIZZLE_EVENT_TYPE_QUERY",
    "DRIZZLE_EVENT_TYPE_STOP",
    "DRIZZLE_EVENT_TYPE_ROTATE",
    "DRIZZLE_EVENT_TYPE_INTVAR",
    "DRIZZLE_EVENT_TYPE_LOAD",
    "DRIZZLE_EVENT_TYPE_SLAVE",
    "DRIZZLE_EVENT_TYPE_CREATE_FILE",
    "DRIZZLE_EVENT_TYPE_APPEND_BLOCK",
    "DRIZZLE_EVENT_TYPE_EXEC_LOAD",
    "DRIZZLE_EVENT_TYPE_DELETE_FILE",
    "DRIZZLE_EVENT_TYPE_NEW_LOAD",
    "DRIZZLE_EVENT_TYPE_RAND",
    "DRIZZLE_EVENT_TYPE_USER_VAR",
    "DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION",
    "DRIZZLE_EVENT_TYPE_XID",
    "DRIZZLE_EVENT_TYPE_BEGIN_LOAD_QUERY",
    "DRIZZLE_EVENT_TYPE_EXECUTE_LOAD_QUERY",
    "DRIZZLE_EVENT_TYPE_TABLE_MAP",
    // "N"ext 3 are used in 5.1.0 -> 5.1.15 only
    "DRIZZLE_EVENT_TYPE_OBSOLETE_WRITE_ROWS",
    "DRIZZLE_EVENT_TYPE_OBSOLETE_UPDATE_ROWS",
    "DRIZZLE_EVENT_TYPE_OBSOLETE_DELETE_ROWS",
    "DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS",
    "DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS",
    "DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS",
    "DRIZZLE_EVENT_TYPE_INCIDENT",
    "DRIZZLE_EVENT_TYPE_HEARTBEAT",
    "DRIZZLE_EVENT_TYPE_IGNORABLE",
    "DRIZZLE_EVENT_TYPE_ROWS_QUERY",
    "DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS",
    "DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS",
    "DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS",
    "DRIZZLE_EVENT_TYPE_GTID",
    "DRIZZLE_EVENT_TYPE_ANONYMOUS_GTID",
    "DRIZZLE_EVENT_TYPE_PREVIOUS_GTIDS",
    "DRIZZLE_EVENT_TYPE_END"
  };

extern void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context);
extern void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context)
{
  (void)context;
  ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "%s(%s)", drizzle_error(connection),
             drizzle_strerror(ret));
}

extern void binlog_event(drizzle_binlog_event_st *event, void *context);
extern void binlog_event(drizzle_binlog_event_st *event, void *context)
{
  binlog_context *binlog_cxt = context;
  uint32_t timestamp;
  timestamp = drizzle_binlog_event_timestamp(event);
  ASSERT_EQ(0, drizzle_binlog_event_timestamp(NULL));
  /* Test to see if timestamp is greater than 2012-01-01 00:00:00, corrupted
   * timestamps will have weird values that shoud fail this after several
   * events.  Also rotate event doesn't have a timestamp so need to add 0
   * to this test */
  ASSERT_FALSE_(((timestamp < 1325376000) && (timestamp != 0)),
                "Bad timestamp retrieved: %u", timestamp);

  ASSERT_EQ(0, drizzle_binlog_event_server_id(NULL));
  ASSERT_NEQ(0, drizzle_binlog_event_server_id(event));

  ASSERT_EQ(0, drizzle_binlog_event_length(NULL));
  ASSERT_NEQ(0, drizzle_binlog_event_length(event));

  ASSERT_EQ(0, drizzle_binlog_event_next_pos(NULL));
  ASSERT_EQ(0, drizzle_binlog_event_raw_length(NULL));

  ASSERT_EQ(0, drizzle_binlog_event_length(NULL));

  if ( binlog_cxt->next_pos > 0 )
  {
    ASSERT_EQ(binlog_cxt->next_pos + drizzle_binlog_event_raw_length(event),
      drizzle_binlog_event_next_pos(event));
  }
  binlog_cxt->next_pos = drizzle_binlog_event_next_pos(event);

  /* An event higher than the max known is bad, either we don't know about
   * new events or type is corrupted */
  ASSERT_EQ_(DRIZZLE_EVENT_TYPE_UNKNOWN, drizzle_binlog_event_type(NULL),
    "binlog event type not defined for event=NULL");
  ASSERT_FALSE_((drizzle_binlog_event_type(event) >= DRIZZLE_EVENT_TYPE_END),
                "Bad event type: %d", drizzle_binlog_event_type(event));
  ASSERT_NULL_(drizzle_binlog_event_data(NULL), "binlog event is NULL");
  ASSERT_NOT_NULL_(drizzle_binlog_event_data(event),
    "binlog event data is NULL");
  ASSERT_NULL_(drizzle_binlog_event_raw_data(NULL), "binlog event is NULL");
  ASSERT_NOT_NULL_(drizzle_binlog_event_raw_data(event),
    "binlog event raw data is NULL");

  ASSERT_EQ(0, drizzle_binlog_event_flags(NULL));
  drizzle_binlog_event_flags(event);
  ASSERT_EQ(0, drizzle_binlog_event_extra_flags(NULL));
  drizzle_binlog_event_extra_flags(event);
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_binlog_st *binlog;
  drizzle_return_t ret;
  binlog_context *binlog_cxt = (binlog_context*) malloc(sizeof(binlog_context));
  binlog_cxt->next_pos = 0;

  // Test initialization of binlog with socket owner as client
  opts = drizzle_options_create();
  drizzle_options_set_socket_owner(opts, DRIZZLE_SOCKET_OWNER_NATIVE);

  set_up_connection();

  binlog = drizzle_binlog_init(NULL, binlog_event, binlog_error, NULL, true);
  ASSERT_NULL_(binlog, "Drizzle connection is null");

  binlog = drizzle_binlog_init(con, NULL, binlog_error, NULL, true);
  ASSERT_NULL_(binlog, "Binlog event callback function is NULL");
  drizzle_binlog_free(binlog);

  binlog = drizzle_binlog_init(con, binlog_event, NULL, NULL, true);
  ASSERT_NULL_(binlog, "Binlog error callback function is NULL");
  drizzle_binlog_free(binlog);

  // Quit the drizzle connection and test binlog initialization with socket
  // owner as native
  close_connection_on_exit();

  opts = drizzle_options_create();
  drizzle_options_set_socket_owner(opts, DRIZZLE_SOCKET_OWNER_NATIVE);
  ASSERT_EQ(DRIZZLE_SOCKET_OWNER_NATIVE, drizzle_options_get_socket_owner(opts));

  set_up_connection();

  char *binlog_file;
  uint32_t end_position;
  drizzle_set_verbose(con, DRIZZLE_VERBOSE_INFO);
  ret = drizzle_binlog_get_filename(NULL, &binlog_file, &end_position, -1);
  ASSERT_EQ(ret, DRIZZLE_RETURN_INVALID_ARGUMENT);
  ret = drizzle_binlog_get_filename(con, &binlog_file, &end_position, -999);
  ASSERT_EQ(ret, DRIZZLE_RETURN_INVALID_ARGUMENT);
  ret = drizzle_binlog_get_filename(con, &binlog_file, &end_position, INT32_MAX);
  ASSERT_EQ(ret, DRIZZLE_RETURN_INVALID_ARGUMENT);

  ret = drizzle_binlog_get_filename(con, &binlog_file, &end_position, -1);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "Couldn't retrieve binlog filename: %s(%s)",
             drizzle_error(con), drizzle_strerror(ret));

  ASSERT_EQ_(DRIZZLE_RETURN_INVALID_ARGUMENT,
    drizzle_binlog_start(binlog, 0, binlog_file, 0),
    "Can't start uninitialized binlog object");

  binlog = drizzle_binlog_init(con, binlog_event, binlog_error,
    (void*) binlog_cxt, true);
  ASSERT_NOT_NULL_(binlog, "Binlog object creation error");
  ret = drizzle_binlog_start(binlog, 0, binlog_file, 0);

  SKIP_IF_(ret == DRIZZLE_RETURN_ERROR_CODE, "Binlog is not open?: %s(%s)",
           drizzle_error(con), drizzle_strerror(ret));
  ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "Drizzle binlog start failure: %s(%s)",
             drizzle_error(con), drizzle_strerror(ret));

  free(binlog_file);
  free(binlog_cxt);

  for (uint i = 0; i < 37; i++)
  {
    const char *actual = drizzle_binlog_event_type_str(binlog_event_types[i]);
    ASSERT_STREQ_(binlog_event_types_name[i], actual, "%s != %s",
      binlog_event_types_name[i], actual);
  }
  ASSERT_STREQ_(drizzle_binlog_event_type_str(999), "DRIZZLE_EVENT_TYPE_UNKNOWN", "Unknown event type");

  return EXIT_SUCCESS;
}
