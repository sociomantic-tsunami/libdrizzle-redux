/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2013 Drizzle Developer Group
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

#include "tests/unit/common.h"

#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  con= drizzle_create(getenv("MYSQL_SOCK"),
                      0,
                      getenv("MYSQL_USER"),
                      getenv("MYSQL_PASSWORD"),
                      getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_return_t ret= drizzle_connect(con);
  SKIP_IF_(ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", drizzle_error(con), drizzle_strerror(ret));
  atexit(close_connection_on_exit);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s socket: %s", drizzle_strerror(ret), getenv("MYSQL_SOCK"));

  drizzle_query(con, "SELECT 1", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "SELECT 1 (%s)", drizzle_error(con));

  // Now that we know everything is good... lets push it.
  drizzle_close(con);

  int limit= 20;
  while (--limit)
  {
    ret= drizzle_connect(con);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s(%s)", drizzle_error(con), drizzle_strerror(ret));

    drizzle_query(con, "SELECT 1", 0, &ret);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "SELECT 1 (%s)", drizzle_error(con));

    // Now that we know everything is good... lets push it.
    drizzle_close(con);
  }

  return EXIT_SUCCESS;
}
