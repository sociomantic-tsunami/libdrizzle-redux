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
#include <string.h>
#include <inttypes.h>
#include <string.h>

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_stmt_st *stmt= NULL;

  ASSERT_EQ(UINT64_MAX, drizzle_stmt_row_count(stmt));

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
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_connect(): %s(%s)", drizzle_error(con), drizzle_strerror(ret));

  drizzle_query(con, "DROP SCHEMA IF EXISTS libdrizzle", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA libdrizzle (%s)", drizzle_error(con));

  drizzle_query(con, "CREATE SCHEMA libdrizzle", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA libdrizzle (%s)", drizzle_error(con));

  ret= drizzle_select_db(con, "libdrizzle");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE libdrizzle");

  drizzle_query(con, "create table libdrizzle.t1 (a int)", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "create table libdrizzle.t1 (a int): %s", drizzle_error(con));

  drizzle_query(con, "insert into libdrizzle.t1 values (1),(2),(3)", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  const char *query= "select * from libdrizzle.t1 where a > ?";
  stmt= drizzle_stmt_prepare(con, query, strlen(query), &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  /* Query should have 1 param */
  ASSERT_EQ_(1, drizzle_stmt_param_count(stmt), "Retrieved bad param count");

  uint32_t val= 1;
  ret = drizzle_stmt_set_int(stmt, 0, val, false);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  ret = drizzle_stmt_execute(stmt);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  ret = drizzle_stmt_buffer(stmt);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  /* Result should have 2 rows */
  uint64_t count = drizzle_stmt_row_count(stmt);
  ASSERT_EQ_(2, count, "%s", drizzle_error(con));

  uint32_t i= 1;
  while (drizzle_stmt_fetch(stmt) != DRIZZLE_RETURN_ROW_END)
  {
    uint32_t res_val;
    const char* char_val;
    char comp_val[3];
    size_t len;
    res_val= drizzle_stmt_get_int(stmt, 0, &ret);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_stmt_get_int");
    char_val= drizzle_stmt_get_string(stmt, 0, &len, &ret);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_stmt_get_string");
    i++;
    if (res_val != i)
    {
      printf("Retrieved unexpected int value\n");
      return EXIT_FAILURE;
    }
    res_val= drizzle_stmt_get_int_from_name(stmt, "a", &ret);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_stmt_get_int (char col name)");
    if (res_val != i)
    {
      printf("Rerieved unexpected int value with char col name\n");
      return EXIT_FAILURE;
    }
    snprintf(comp_val, 3, "%"PRIu32, i);
    if (strcmp(comp_val, char_val) != 0)
    {
      printf("Retrieved unexpected string value\n");
      return EXIT_FAILURE;
    }
  }
  /* Should have cycled through 2 rows (values 2 and 3) */
  if (i != 3)
  {
    printf("Retrieved bad number of rows\n");
    return EXIT_FAILURE;
  }
  ret = drizzle_stmt_close(stmt);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_error(con));

  drizzle_query(con, "DROP TABLE libdrizzle.t1", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP TABLE libdrizzle.t1");

  drizzle_query(con, "DROP SCHEMA IF EXISTS libdrizzle", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP SCHEMA libdrizzle (%s)", drizzle_error(con));

  ret= drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_strerror(ret));

  return EXIT_SUCCESS;
}
