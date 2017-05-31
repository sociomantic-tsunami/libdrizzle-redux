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

/**
 * @brief Tests that drizzle_st::event_watch_fn is set correctly when calling
 * ::drizzle_set_event_watch_fn() and called with valid arguments
 *
 * @details Check that the drizzle_event_callback was called. It is done
 * indirectly by asserting that cxt_b has been incremented at three times.
 * The number of calls varies with the availability of underlying socket.
 * However a successful run of the unittest is guaranteed to invoke the
 * event_callback on the following state changes:
 *
 * 1. when drizzle connection is initiated by calling drizzle_connect
 * 2. before connecting to server after 1 succeeds
 * 3. when the drizzle_st connection is closed by calling drizzle_quit
 */

#include <yatl/lite.h>

#include <libdrizzle-redux/libdrizzle.h>
#include <libdrizzle-redux/constants.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern drizzle_return_t drizzle_event_callback(drizzle_st *con, short events,
                                               void *context);
extern drizzle_return_t drizzle_event_callback(drizzle_st *con, short events,
  void *context)
{
  ASSERT_TRUE((events != -1));
  char* char_ptr = (char*) drizzle_context(con);
  ASSERT_NOT_NULL_(char_ptr, "Drizzle connection context is null");
  ASSERT_STREQ_("Context is everything", char_ptr,
    "Invalid drizzle context: Got '%s', Expected 'Context is everything'",
    char_ptr);
  printf("%s\n", char_ptr);

  int* cxt_ptr = (int*)context;

  ASSERT_NOT_NULL_(cxt_ptr, "Drizzle event callback context is null");
  (*cxt_ptr)++;
  printf("Drizzle event callback number %d\n", cxt_ptr[0]);

  return DRIZZLE_RETURN_OK;
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  char cxt_a[] = "Context is everything";
  int cxt_b = 0;

  drizzle_st *con = drizzle_create(getenv("MYSQL_SERVER"),
                                   getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                                        : DRIZZLE_DEFAULT_TCP_PORT,
                                   getenv("MYSQL_USER"),
                                   getenv("MYSQL_PASSWORD"),
                                   getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  // Set drizzle dummy context
  drizzle_set_context(con, (void *)&cxt_a);
  ASSERT_NOT_NULL_(drizzle_context(con), "Drizzle context is null");

  // Set user defined callback function event_watch_fn
  drizzle_set_event_watch_fn(con, drizzle_event_callback, (void *)&cxt_b);

  drizzle_return_t driz_ret = drizzle_connect(con);
  SKIP_IF_(driz_ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s",
    drizzle_strerror(driz_ret));
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s(%s)", drizzle_error(con),
    drizzle_strerror(driz_ret));

  driz_ret = drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s", drizzle_strerror(driz_ret));
  ASSERT_EQ_(cxt_b > 0, true, "Unexpected number of event callbacks, Got '%d', "
    "Expected '1' or more", cxt_b);

  printf("\nEvent callback was invoked %d times\n", cxt_b);

  return EXIT_SUCCESS;
}
