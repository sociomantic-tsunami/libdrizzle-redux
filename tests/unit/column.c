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
#include "tests/unit/common.h"

#include <libdrizzle-redux/libdrizzle.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const drizzle_column_type_t column_types[] = {
    DRIZZLE_COLUMN_TYPE_DECIMAL,     DRIZZLE_COLUMN_TYPE_TINY,
    DRIZZLE_COLUMN_TYPE_SHORT,       DRIZZLE_COLUMN_TYPE_LONG,
    DRIZZLE_COLUMN_TYPE_FLOAT,       DRIZZLE_COLUMN_TYPE_DOUBLE,
    DRIZZLE_COLUMN_TYPE_NULL,        DRIZZLE_COLUMN_TYPE_TIMESTAMP,
    DRIZZLE_COLUMN_TYPE_LONGLONG,    DRIZZLE_COLUMN_TYPE_INT24,
    DRIZZLE_COLUMN_TYPE_DATE,        DRIZZLE_COLUMN_TYPE_TIME,
    DRIZZLE_COLUMN_TYPE_DATETIME,    DRIZZLE_COLUMN_TYPE_YEAR,
    DRIZZLE_COLUMN_TYPE_NEWDATE,     DRIZZLE_COLUMN_TYPE_VARCHAR,
    DRIZZLE_COLUMN_TYPE_BIT,         DRIZZLE_COLUMN_TYPE_TIMESTAMP2,
    DRIZZLE_COLUMN_TYPE_DATETIME2,   DRIZZLE_COLUMN_TYPE_TIME2,
    DRIZZLE_COLUMN_TYPE_NEWDECIMAL,  DRIZZLE_COLUMN_TYPE_ENUM,
    DRIZZLE_COLUMN_TYPE_SET,         DRIZZLE_COLUMN_TYPE_TINY_BLOB,
    DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB, DRIZZLE_COLUMN_TYPE_LONG_BLOB,
    DRIZZLE_COLUMN_TYPE_BLOB,        DRIZZLE_COLUMN_TYPE_VAR_STRING,
    DRIZZLE_COLUMN_TYPE_STRING,      DRIZZLE_COLUMN_TYPE_GEOMETRY
  };

const char *column_type_names[] = {
    "DECIMAL",    "TINY", "SHORT",      "LONG",      "FLOAT",
    "DOUBLE",     "NULL", "TIMESTAMP",  "LONGLONG",  "INT24",
    "DATE",       "TIME", "DATETIME",   "YEAR",      "NEWDATE",
    "VARCHAR",    "BIT",  "TIMESTAMP2", "DATETIME2", "TIME2",
    "NEWDECIMAL", "ENUM", "SET",        "TINY_BLOB", "MEDIUM_BLOB",
    "LONG_BLOB",  "BLOB", "VAR_STRING", "STRING",    "GEOMETRY"};

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_row_t row;
  int num_fields;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;

  set_up_connection();
  CHECKED_QUERY("SET CHARACTER SET latin1"); /* Or any other one-byte-per-character
                                                character set */
  set_up_schema("test_column");

  CHECKED_QUERY("CREATE TABLE test_column.t1 (a INT PRIMARY KEY AUTO_INCREMENT, "
		"b VARCHAR(255), c TIMESTAMP DEFAULT CURRENT_TIMESTAMP)");

  CHECKED_QUERY("INSERT INTO test_column.t1 (b) VALUES ('this'),('is'),('war')");

  CHECKED_QUERY("SELECT * FROM test_column.t1");

  drizzle_result_buffer(result);
  num_fields = drizzle_result_column_count(result);

  ASSERT_EQ_(num_fields, 3, "Retrieved bad number of fields");

  int i = 0;
  drizzle_column_st *column;
  while ((row = drizzle_row_next(result)))
  {
    drizzle_column_seek(result, 0);
    int cur_column = 0;
    i++;
    char buf[10];
    snprintf(buf, sizeof(buf), "%d", i);
    ASSERT_EQ_(strcmp(row[0], buf), 0, "Retrieved bad row value");
    while ((column= drizzle_column_next(result)))
    {
      cur_column++;
      ASSERT_EQ_(strcmp(drizzle_column_db(column), "test_column"), 0,
                 "Column has bad DB name");
      ASSERT_EQ_(strcmp(drizzle_column_table(column), "t1"), 0,
                 "Column had bad table name");
      switch (cur_column) {
      case 1:
        ASSERT_EQ_(drizzle_column_type(column), DRIZZLE_COLUMN_TYPE_LONG,
                   "Column type wrong");
        break;
      case 2:
        ASSERT_EQ_(drizzle_column_max_size(column), 255,
                   "Column max size wrong %lu != 255",
                   drizzle_column_max_size(column));

        ASSERT_EQ_(drizzle_column_charset(column), DRIZZLE_CHARSET_LATIN1_SWEDISH_CI,
                   "Column charset wrong, %d != %d", drizzle_column_charset(column),
                   DRIZZLE_CHARSET_LATIN1_SWEDISH_CI);
        break;
      case 3:
        ASSERT_EQ_(drizzle_column_type(column), DRIZZLE_COLUMN_TYPE_TIMESTAMP,
                   "Column type wrong");
        break;
      }
    }
    ASSERT_EQ_(cur_column, 3, "Wrong column count");
  }
  /* Should have had 3 rows */
  ASSERT_EQ_(i, 3, "Retrieved bad number of rows");

  drizzle_result_free(result);

  CHECKED_QUERY("DROP TABLE test_column.t1");

  tear_down_schema("test_column");

  /* Check that column type is resolved to the correct string representation */
  for (i = 0; i < 30; i++)
  {
    ASSERT_STREQ_(drizzle_column_type_str(column_types[i]), column_type_names[i],
                  "Column type 'DRIZZLE_COLUMN_TYPE_%s' resolved to wrong name: '%s'",
                  column_type_names[i], drizzle_column_type_str(column_types[i]));
  }

  return EXIT_SUCCESS;
}
