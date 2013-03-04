/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012 Drizzle Developer Group
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

#define CHECKED_QUERY(cmd) drizzle_query(con, cmd, 0, &ret); ASSERT_EQ_(ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", drizzle_strerror(ret), drizzle_error(con), cmd);

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;

  drizzle_st *con= drizzle_create(getenv("MYSQL_SERVER"),
                                  getenv("MYSQL_PORT") ? atoi("MYSQL_PORT") : DRIZZLE_DEFAULT_TCP_PORT,
                                  getenv("MYSQL_USER"),
                                  getenv("MYSQL_PASSWORD"),
                                  getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_return_t ret= drizzle_connect(con);
  if (ret == DRIZZLE_RETURN_COULD_NOT_CONNECT)
  {
    char error[DRIZZLE_MAX_ERROR_SIZE];
    strncpy(error, drizzle_error(con), DRIZZLE_MAX_ERROR_SIZE);
    drizzle_quit(con);

    SKIP_IF_(ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", error, drizzle_strerror(ret));
  }
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  CHECKED_QUERY("DROP SCHEMA IF EXISTS libdrizzle");

  CHECKED_QUERY("CREATE SCHEMA libdrizzle");

  ret= drizzle_select_db(con, "libdrizzle");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE libdrizzle");

  CHECKED_QUERY("create table libdrizzle.t1 (a int primary key auto_increment, b int)");

  drizzle_result_st *result= drizzle_query(con, "insert into libdrizzle.t1 (b) values (1),(2),(3)", 0, &ret);
  ASSERT_EQ_(ret, DRIZZLE_RETURN_OK, "insert into libdrizzle.t1 (b) values (1),(2),(3): %s", drizzle_error(con));
  ASSERT_TRUE(result);

  ASSERT_EQ_(drizzle_result_insert_id(result), 1, "Got bad insert_id (expected 1, got %"PRIu64")", drizzle_result_insert_id(result));
  drizzle_result_free(result);

  result= drizzle_query(con, "INSERT INTO libdrizzle.t1 (b) VALUES (4),(5),(6)", 0, &ret);
  ASSERT_EQ_(ret, DRIZZLE_RETURN_OK, "INSERT INTO libdrizzle.t1 (b) VALUES (4),(5),(6): %s", drizzle_error(con));

  ASSERT_EQ_(drizzle_result_insert_id(result), 4, "Got bad insert_id (expected 4, got %"PRIu64")", drizzle_result_insert_id(result));
  drizzle_result_free(result);

  CHECKED_QUERY("DROP TABLE libdrizzle.t1");

  CHECKED_QUERY("DROP SCHEMA libdrizzle");

  ret= drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_strerror(ret));

  return EXIT_SUCCESS;
}
