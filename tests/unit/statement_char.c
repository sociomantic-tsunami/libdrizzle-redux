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

#include <inttypes.h>
#include <libdrizzle-5.1/libdrizzle.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_stmt_st *stmt;

  drizzle_st *con = drizzle_create(getenv("MYSQL_SERVER"),
                                   getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                                        : DRIZZLE_DEFAULT_TCP_PORT,
                                   getenv("MYSQL_USER"),
                                   getenv("MYSQL_PASSWORD"),
                                   getenv("MYSQL_SCHEMA"), 0);

  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_return_t ret = drizzle_connect(con);
  if (ret == DRIZZLE_RETURN_COULD_NOT_CONNECT)
  {
    char error[DRIZZLE_MAX_ERROR_SIZE];
    strncpy(error, drizzle_error(con), DRIZZLE_MAX_ERROR_SIZE);
    drizzle_quit(con);

    SKIP_IF_(ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", error,
             drizzle_strerror(ret));
  }
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_connect(): %s(%s)",
             drizzle_error(con), drizzle_strerror(ret));

  drizzle_query(con, "DROP SCHEMA IF EXISTS test_stmt_ch", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA test_stmt_ch (%s)",
             drizzle_error(con));

  drizzle_query(con, "CREATE SCHEMA test_stmt_ch", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA test_stmt_ch (%s)",
             drizzle_error(con));

  ret = drizzle_select_db(con, "test_stmt_ch");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE test_stmt_ch");

  drizzle_query(con, "CREATE TABLE test_stmt_ch.t1 (a VARCHAR(50))", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE TABLE test_stmt_ch.t1 (a INT): %s",
             drizzle_error(con));

  drizzle_query(con,
	        "INSERT INTO test_stmt_ch.t1 VALUES ('hello'),('drizzle'),('people')",
		0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  const char *query = "SELECT * FROM test_stmt_ch.t1 WHERE a = ?";
  stmt = drizzle_stmt_prepare(con, query, strlen(query), &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  /* Query should have 1 param */
  ASSERT_EQ_(1, drizzle_stmt_param_count(stmt), "Retrieved bad param count");

  char val[] = "hello";
  ret = drizzle_stmt_set_string(stmt, 0, val, strlen(val));
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Bind failure\n");
    return EXIT_FAILURE;
  }

  ret = drizzle_stmt_execute(stmt);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Execute failure\n");
    return EXIT_FAILURE;
  }
  ret = drizzle_stmt_buffer(stmt);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Buffer failure\n");
    return EXIT_FAILURE;
  }
  /* Result should have 1 row */
  if (drizzle_stmt_row_count(stmt) != 1)
  {
    printf("Retrieved bad row count\n");
    return EXIT_FAILURE;
  }
  uint32_t i = 0;
  while (drizzle_stmt_fetch(stmt) != DRIZZLE_RETURN_ROW_END)
  {
    const char *char_val;
    size_t len;
    char_val = drizzle_stmt_get_string(stmt, 0, &len, &ret);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_stmt_get_string");
    i++;
    if (strncmp(val, char_val, len) != 0)
    {
      printf("Retrieved unexpected string value\n");
      return EXIT_FAILURE;
    }
  }
  /* Should have cycled through 1 row */
  if (i != 1)
  {
    printf("Retrieved bad number of rows\n");
    return EXIT_FAILURE;
  }
  ret = drizzle_stmt_close(stmt);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_stmt_close() %s",
             drizzle_error(con));

  drizzle_query(con, "DROP TABLE test_stmt_ch.t1", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP TABLE test_stmt_ch.t1");

  drizzle_query(con, "DROP SCHEMA IF EXISTS test_stmt_ch", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP SCHEMA test_stmt_ch (%s)",
             drizzle_error(con));

  ret = drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_strerror(ret));

  return EXIT_SUCCESS;
}
