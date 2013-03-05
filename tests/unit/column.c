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
#include "tests/unit/cleanup.h"

#include <libdrizzle-5.1/libdrizzle.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#define CHECKED_QUERY(cmd) drizzle_query(con, cmd, 0, &ret); ASSERT_EQ_(ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", drizzle_strerror(ret), drizzle_error(con), cmd);

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_row_t row;
  int num_fields;

  con= drizzle_create(getenv("MYSQL_SERVER"),
                      getenv("MYSQL_PORT") ? atoi("MYSQL_PORT") : DRIZZLE_DEFAULT_TCP_PORT,
                      getenv("MYSQL_USER"),
                      getenv("MYSQL_PASSWORD"),
                      getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");
  CLOSE_ON_EXIT(con);

  drizzle_return_t ret= drizzle_connect(con);
  SKIP_IF_(ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s", drizzle_strerror(ret));
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s(%s)", drizzle_error(con), drizzle_strerror(ret));

  CHECKED_QUERY("DROP SCHEMA IF EXISTS libdrizzle");

  CHECKED_QUERY("SET CHARACTER SET latin1"); /* Or any other one-byte-per-character character set */
    
  CHECKED_QUERY("CREATE SCHEMA libdrizzle");

  ret= drizzle_select_db(con, "libdrizzle");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE libdrizzle");

  CHECKED_QUERY("create table libdrizzle.t1 (a int primary key auto_increment, b varchar(255), c timestamp default current_timestamp)");

  CHECKED_QUERY("insert into libdrizzle.t1 (b) values ('this'),('is'),('war')");

  drizzle_result_st *result= drizzle_query(con, "select * from libdrizzle.t1", 0, &ret);
  ASSERT_TRUE_(ret == DRIZZLE_RETURN_OK, "select * from libdrizzle.t1");

  drizzle_result_buffer(result);
  num_fields= drizzle_result_column_count(result);

  ASSERT_EQ_(num_fields, 3, "Retrieved bad number of fields");

  int i= 0;
  drizzle_column_st *column;
  while ((row = drizzle_row_next(result)))
  {
    drizzle_column_seek(result, 0);
    int cur_column= 0;
    i++;
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", i);
    ASSERT_EQ_(strcmp(row[0], buf), 0, "Retrieved bad row value");
    while ((column= drizzle_column_next(result)))
    {
      cur_column++;
      ASSERT_EQ_(strcmp(drizzle_column_db(column), "libdrizzle"), 0, "Column has bad DB name");
      ASSERT_EQ_(strcmp(drizzle_column_table(column), "t1"), 0, "Column had bad table name");
        switch (cur_column) {
            case 1:
                ASSERT_EQ_(drizzle_column_type(column), DRIZZLE_COLUMN_TYPE_LONG, "Column type wrong");
                break;
            case 2:
                ASSERT_EQ_(drizzle_column_max_size(column), 255, "Column max size wrong %zu != 255", drizzle_column_max_size(column));
                
                ASSERT_EQ_(drizzle_column_charset(column), DRIZZLE_CHARSET_LATIN1_SWEDISH_CI, "Column charset wrong, %d != %d", drizzle_column_charset(column), DRIZZLE_CHARSET_UTF8_BIN);
                break;
            case 3:
                ASSERT_EQ_(drizzle_column_type(column), DRIZZLE_COLUMN_TYPE_TIMESTAMP, "Column type wrong");
                break;
        }
    }
    ASSERT_EQ_(cur_column, 3, "Wrong column count");
  }
  /* Should have had 3 rows */
  ASSERT_EQ_(i, 3, "Retrieved bad number of rows");

  drizzle_result_free(result);

  drizzle_query(con, "DROP TABLE libdrizzle.t1", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP TABLE libdrizzle.t1");

  drizzle_query(con, "DROP SCHEMA IF EXISTS libdrizzle", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP SCHEMA libdrizzle (%s)", drizzle_error(con));

  return EXIT_SUCCESS;
}
