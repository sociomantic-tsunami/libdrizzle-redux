/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012 Andrew Hutchings (andrew@linuxjedi.co.uk)
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

#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  drizzle_st *con= drizzle_create_tcp(getenv("MYSQL_SERVER"),
                                      getenv("MYSQL_PORT") ? atoi("MYSQL_PORT") : DRIZZLE_DEFAULT_TCP_PORT,
                                      getenv("MYSQL_USER"),
                                      getenv("MYSQL_PASSWORD"),
                                      getenv("MYSQL_SCHEMA"), 0);

  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_return_t ret= drizzle_connect(con);
  if (ret == DRIZZLE_RETURN_COULD_NOT_CONNECT)
  {
    const char *error= drizzle_error(con);
    drizzle_quit(con);
    SKIP_IF_(ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", error, drizzle_strerror(ret));
  }
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s(%s)", drizzle_error(con), drizzle_strerror(ret));

  drizzle_result_st *result= drizzle_start_binlog(con, 0, "", 0, &ret);
  ASSERT_TRUE(result);
  SKIP_IF_(ret == DRIZZLE_RETURN_ERROR_CODE, "Binlog is not open?: %s(%s)", drizzle_error(con), drizzle_strerror(ret));
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "Drizzle binlog start failure: %s(%s)", drizzle_error(con), drizzle_strerror(ret));

  while (ret == DRIZZLE_RETURN_OK)
  {
    uint32_t timestamp;
    ret= drizzle_binlog_get_next_event(result);
    if (ret == DRIZZLE_RETURN_EOF)
    {
      break;
    }
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "Binlog error %s\n", drizzle_error(con));
    timestamp= drizzle_binlog_event_timestamp(result);
    /* Test to see if timestamp is greater than 2012-01-01 00:00:00, corrupted
     * timestamps will have weird values that shoud fail this after several
     * events.  Also rotate event doesn't have a timestamp so need to add 0
     * to this test */
    ASSERT_FALSE_(((timestamp < 1325376000) && (timestamp != 0)), "Bad timestamp retrieved: %" PRIu32, timestamp);

    /* An event higher than the max known is bad, either we don't know about
     * new events or type is corrupted */
    ASSERT_FALSE_((drizzle_binlog_event_type(result) >= DRIZZLE_EVENT_TYPE_END), "Bad event type: %d", drizzle_binlog_event_type(result));
  }

  ret= drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_strerror(ret));

  return EXIT_SUCCESS;
}
