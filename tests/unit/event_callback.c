/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2016 Sociomantic Labs GmbH
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
#include <libdrizzle-5.1/constants.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern drizzle_return_t drizzle_event_callback(drizzle_st *con, short events,
  void *context);
extern drizzle_return_t drizzle_event_callback(drizzle_st *con, short events,
  void *context)
{
  ASSERT_TRUE((events != -1));
  int* cxt_ptr = (int*) drizzle_context(con);
  ASSERT_NOT_NULL_(cxt_ptr, "Drizzle connection context is null");
  ASSERT_EQ_(1, cxt_ptr[0], "Invalid drizzle context: Got '%d', Expected '1'",
    cxt_ptr[0]);

  cxt_ptr = (int*)context;
  ASSERT_NOT_NULL_(cxt_ptr, "Drizzle Event Callback context is null");
  (*cxt_ptr)++;

  return DRIZZLE_RETURN_OK;
}

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  int cxt_a = 1;
  int cxt_b = 0;

  drizzle_st *con= drizzle_create(getenv("MYSQL_SERVER"),
                                  getenv("MYSQL_PORT") ? atoi("MYSQL_PORT") :
                                    DRIZZLE_DEFAULT_TCP_PORT,
                                  getenv("MYSQL_USER"),
                                  getenv("MYSQL_PASSWORD"),
                                  getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_set_context(con, (void*)&cxt_a);
  ASSERT_NOT_NULL_(drizzle_context(con) , "Drizzle context is null");

  drizzle_set_event_watch_fn(con, drizzle_event_callback, (void*)&cxt_b);

  drizzle_return_t driz_ret= drizzle_connect(con);
  SKIP_IF_(driz_ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s",
    drizzle_strerror(driz_ret));
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s(%s)", drizzle_error(con),
    drizzle_strerror(driz_ret));

  driz_ret= drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s", drizzle_strerror(driz_ret));

  // Check that the drizzle_event_callback was called. It is done indirectly by
  // asserting that cxt_b has been incremented 4 times.
  // The increments of cxt_b correspond to the following state changes:
  //
  // 1. when drizzle connection is initied by calling drizzle_connect
  // 2. while connecting to server
  // 3. when server handshake succeeds
  // 4. when the drizzle_st connection is closed by calling drizzle_quit
  //
  // Please note that stepping through the test with 'make gdb-event_callback'
  // can cause the test to fail
  ASSERT_TRUE(cxt_b >= 3);

  return EXIT_SUCCESS;
}
