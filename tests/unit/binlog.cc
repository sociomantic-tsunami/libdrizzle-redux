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

#include <libdrizzle-5.1/libdrizzle.h>

#include "tests/unit/common.h"

#include <cstdio>
#include <cstdlib>

void binlog_error(drizzle_return_t ret, drizzle_st *connection, void *context)
{
  (void) context;
  ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "%s(%s)", drizzle_error(connection), drizzle_strerror(ret));
}

void binlog_event(drizzle_binlog_event_st *event, void *context)
{
  (void) context;
  uint32_t timestamp;
  timestamp= drizzle_binlog_event_timestamp(event);
  /* Test to see if timestamp is greater than 2012-01-01 00:00:00, corrupted
   * timestamps will have weird values that shoud fail this after several
   * events.  Also rotate event doesn't have a timestamp so need to add 0
   * to this test */
  ASSERT_FALSE_(((timestamp < 1325376000) && (timestamp != 0)), "Bad timestamp retrieved: %u", timestamp);

  /* An event higher than the max known is bad, either we don't know about
   * new events or type is corrupted */
  ASSERT_FALSE_((drizzle_binlog_event_type(event) >= DRIZZLE_EVENT_TYPE_END), "Bad event type: %d", drizzle_binlog_event_type(event));
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_binlog_st *binlog;
  drizzle_return_t ret;

  set_up_connection();
  
  binlog= drizzle_binlog_init(con, binlog_event, binlog_error, NULL, true);
  ret= drizzle_binlog_start(binlog, 0, "", 0);
  SKIP_IF_(ret == DRIZZLE_RETURN_ERROR_CODE, "Binlog is not open?: %s(%s)", drizzle_error(con), drizzle_strerror(ret));
  ASSERT_EQ_(DRIZZLE_RETURN_EOF, ret, "Drizzle binlog start failure: %s(%s)", drizzle_error(con), drizzle_strerror(ret));
  return EXIT_SUCCESS;
}
