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

#define _GNU_SOURCE

#include "tests/unit/common.h"
#include <yatl/lite.h>

#include <libdrizzle-redux/libdrizzle.h>

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECKED_QUERY(cmd) \
  result = drizzle_query(con, cmd, 0, &driz_ret); \
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", \
             drizzle_strerror(driz_ret), drizzle_error(con), cmd);

#define CHECK(s) \
  driz_ret = (s); \
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, in \"%s\"", \
             drizzle_strerror(driz_ret), drizzle_error(con), #s);

static const drizzle_column_type_t expected_column_types[10] = {
    DRIZZLE_COLUMN_TYPE_NONE, /* Columns are 1-indexed */
    DRIZZLE_COLUMN_TYPE_LONG,
    DRIZZLE_COLUMN_TYPE_DATE, DRIZZLE_COLUMN_TYPE_YEAR,
    DRIZZLE_COLUMN_TYPE_TIMESTAMP, DRIZZLE_COLUMN_TYPE_TIMESTAMP,
    DRIZZLE_COLUMN_TYPE_TIME, DRIZZLE_COLUMN_TYPE_TIME,
    DRIZZLE_COLUMN_TYPE_DATETIME,
    //  DRIZZLE_COLUMN_TYPE_NEWDATE,
    DRIZZLE_COLUMN_TYPE_DATETIME,
};

static const char *column_names[10] = {NULL, "a", "b", "c", "d",
                                       "e",  "f", "g", "h", "i"};

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;
  drizzle_row_t row;
  int num_fields;
  const char *query;
  drizzle_stmt_st *sth;
  unsigned rows_in_table;

  set_up_connection();
  set_up_schema("test_datetime");

  SKIP_IF_(drizzle_server_version_number(con) < 50604,
           "Test requires MySQL 5.6.4 or higher");

  CHECKED_QUERY("CREATE TABLE test_datetime.dt1"
                "("
                "a INT PRIMARY KEY NOT NULL,"
                "b DATE,"
                "c YEAR(4),"
                "d TIMESTAMP(0),"
                "e TIMESTAMP(6) DEFAULT CURRENT_TIMESTAMP(6),"
                "f TIME(0),"
                "g TIME(6),"
                "h DATETIME(0),"
                "i DATETIME(6)"
                ")");
  rows_in_table = 0;

  /* Insert rows with pk 1 and 2 */
  CHECKED_QUERY("INSERT INTO test_datetime.dt1 (a,b,c,d,e,f,g,h,i) "
                "VALUES (1, '1970-01-01', '2112', '2013-03-13 09:22:00.001',"
                "'2013-03-13 09:22:00.001', '6:15:03', '23:59:59.75',"
                "'1642-12-25 12:15:01', '1642-12-25 12:12:00.125'),"
                "(2, '84-02-29', '12', NOW(), NOW(), '3 6:15:03',"
                "'23:59:59.0625', '1642-12-25 12:15:01', '1642-12-25 "
                "12:12:00.000000')");
  ASSERT_EQ(drizzle_result_affected_rows(result), 2);
  rows_in_table += 2;

  /* Insert row with pk 3 and 4 - test marshaling values we transmit */
  query = "INSERT INTO test_datetime.dt1 (a,b,c,d,e,f,g,h,i) "
          "VALUES (?,?,?,?,?,?,?,?,?)";
  sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);

  /* Row 3 should be the same as row 1, above */
  CHECK(drizzle_stmt_set_short(sth, 0, 3, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 1, 1970, 1, 1, 0, 0, 0, 0));
  CHECK(drizzle_stmt_set_int(sth, 2, 2112, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 3, 2013, 3, 13, 9, 22, 0, 1000));
  CHECK(drizzle_stmt_set_timestamp(sth, 4, 2013, 3, 13, 9, 22, 0, 1000));
  CHECK(drizzle_stmt_set_time(sth, 5, 0, 6, 15, 3, 0, 0));
  CHECK(drizzle_stmt_set_time(sth, 6, 0, 23, 59, 59, 750000, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 7, 1642, 12, 25, 12, 15, 1, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 8, 1642, 12, 25, 12, 12, 0, 125000));

  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  rows_in_table++;

  /* Row 4 is similar to row 2, above. But 2-digit years aren't automatically
   * y2k-promoted if we send them raw. */
  CHECK(drizzle_stmt_set_short(sth, 0, 4, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 1, 84, 2, 9, 0, 0, 0, 0));
  /* 12 will become 2012 because we're sending an int, not a YEAR */
  CHECK(drizzle_stmt_set_int(sth, 2, 12, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 3, 2013, 3, 13, 9, 22, 0, 1000));
  CHECK(drizzle_stmt_set_timestamp(sth, 4, 2013, 3, 13, 9, 22, 0, 1000));
  CHECK(drizzle_stmt_set_time(sth, 5, 3, 6, 15, 3, 0, 0));
  CHECK(drizzle_stmt_set_time(sth, 6, 0, 23, 59, 59, 62500, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 7, 1642, 12, 25, 12, 15, 1, 0));
  CHECK(drizzle_stmt_set_timestamp(sth, 8, 1642, 12, 25, 12, 12, 0, 0));

  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  rows_in_table++;

  CHECK(drizzle_stmt_close(sth));

  /* Read the table back, with values sent over the wire in text form */
  CHECKED_QUERY("SELECT * FROM test_datetime.dt1 ORDER BY a");

  drizzle_result_buffer(result);
  num_fields = drizzle_result_column_count(result);

  ASSERT_EQ_(num_fields, 9, "Retrieved bad number of fields");

  unsigned int cur_row = 0;
  drizzle_column_st *column;
  while ((row = drizzle_row_next(result)))
  {
    drizzle_column_seek(result, 0);
    int cur_column = 0;
    cur_row++;
    ASSERT_EQ(drizzle_row_current(result), cur_row);
    ASSERT_TRUE(cur_row <= 4);

    char expected_colA[10];
    sprintf(expected_colA, "%d", cur_row);
    ASSERT_EQ_(strcmp(row[0], expected_colA), 0,
               "Retrieved bad row value; row=%d got=%s expected=%s", cur_row,
               row[0], expected_colA);

    while ((column= drizzle_column_next(result)))
    {
      cur_column++;
      ASSERT_EQ_(strcmp(drizzle_column_db(column), "test_datetime"), 0,
                 "Column has bad DB name");
      ASSERT_EQ_(strcmp(drizzle_column_table(column), "dt1"), 0,
                 "Column %d had bad table name", cur_column);
      ASSERT_EQ(drizzle_column_current(result), cur_column);
      ASSERT_STREQ_(drizzle_column_name(column), column_names[cur_column],
                    "Column %d name", cur_column);
      ASSERT_EQ_(drizzle_column_type(column), expected_column_types[cur_column],
                 "Column %d has type=%d expected=%d", cur_column,
                 drizzle_column_type(column),
                 expected_column_types[cur_column]);
    }
    ASSERT_EQ_(cur_column, 9, "Wrong column count");

    if (cur_row == 1 || cur_row == 3) {
      ASSERT_STREQ("1970-01-01", row[1]);
      ASSERT_STREQ("2112", row[2]);
      ASSERT_STREQ("06:15:03", row[5]);
      ASSERT_STREQ("23:59:59.750000", row[6]);
      ASSERT_STREQ("1642-12-25 12:15:01", row[7]);
      ASSERT_STREQ("1642-12-25 12:12:00.125000", row[8]);
    }

    if (cur_row == 2) {
      ASSERT_STREQ("1984-02-29", row[1]);
      ASSERT_STREQ("2012", row[2]);
    } else if (cur_row == 4) {
      ASSERT_STREQ("0084-02-09", row[1]);
      ASSERT_STREQ("2012", row[2]);
    }

    if (cur_row == 2 || cur_row == 4) {
      ASSERT_STREQ("78:15:03", row[5]);
      ASSERT_STREQ("23:59:59.062500", row[6]);
      ASSERT_STREQ("1642-12-25 12:15:01", row[7]);
      ASSERT_STREQ("1642-12-25 12:12:00.000000", row[8]);
    }
  }
  ASSERT_EQ_(cur_row, rows_in_table, "Retrieved bad number of rows");

  drizzle_result_free(result);

  /* Read the table back, with values sent over the wire in binary form */
  query = "SELECT a,b,c,d,e,f,g,h,i FROM test_datetime.dt1 ORDER BY a";
  sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);

  ASSERT_EQ(rows_in_table, drizzle_stmt_row_count(sth));
  cur_row = 0;
  while (drizzle_stmt_fetch(sth) != DRIZZLE_RETURN_ROW_END)
  {
    size_t lth;
    const char *col_strval;
    int col_intval;

    cur_row++;
    printf("Row %d\n", cur_row);

    int columnA = drizzle_stmt_get_int(sth, 0, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK,
               "Error (%s): %s, column %d of row %d",
               drizzle_strerror(driz_ret), drizzle_error(con), 1, cur_row);
    ASSERT_EQ(cur_row, (unsigned)columnA);

#define ASSERT_COL_STREQ_(coln, expected, ...) \
  col_strval = drizzle_stmt_get_string(sth, coln - 1, &lth, &driz_ret); \
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Fetching column %u of row %u", \
             coln, cur_row); \
  ASSERT_STREQ_(expected, col_strval, \
                "Stringified value of column %u of row %u", coln, cur_row); \
  ASSERT_EQ_(lth, strlen(col_strval), \
             "Length of stringified value of column %u of row %u", coln, \
             cur_row);

    switch (cur_row) {
    case 1:
    case 3:
      ASSERT_COL_STREQ_(2, "1970-01-01");
      ASSERT_COL_STREQ_(3, "2112");
      ASSERT_COL_STREQ_(6, "06:15:03");
      ASSERT_COL_STREQ_(7, "23:59:59.750000");
      ASSERT_COL_STREQ_(8, "1642-12-25 12:15:01");
      ASSERT_COL_STREQ_(9, "1642-12-25 12:12:00.125000");
      break;
    case 2:
    case 4:
      ASSERT_COL_STREQ_(3, "2012");
      ASSERT_COL_STREQ_(6, "78:15:03");
      ASSERT_COL_STREQ_(7, "23:59:59.062500");
      ASSERT_COL_STREQ_(8, "1642-12-25 12:15:01");
      ASSERT_COL_STREQ_(9, "1642-12-25 12:12:00.000000");
      break;
    }

    if (cur_row == 2) {
      ASSERT_COL_STREQ_(2, "1984-02-29");
    } else if (cur_row == 4) {
      /* Yes, year 84, during the reign of Domitian */
      ASSERT_COL_STREQ_(2, "0084-02-09");
    }

    /* TODO: libdrizzle currently has no way to give us access to the actual
     * returned values for time/date fields. If that changes, test the values
     * here. */

    col_intval = drizzle_stmt_get_int(sth, 3 - 1, &driz_ret);
    ASSERT_EQ(driz_ret, DRIZZLE_RETURN_OK);
    switch (cur_row) {
    case 1:
    case 3:
      ASSERT_EQ(2112, col_intval);
      break;
    case 2:
    case 4:
      ASSERT_EQ(2012, col_intval);
      break;
    }
  }
  ASSERT_EQ(cur_row, rows_in_table);
  driz_ret = drizzle_stmt_close(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s",
             drizzle_strerror(driz_ret), drizzle_error(con));

  /* Check that libdrizzle stringifies values the same way the server does */
  for (unsigned checking_column = 2; checking_column < 10; checking_column++) {
    const char *col_name = column_names[checking_column];
    char query_buf[128];
    int VARIABLE_IS_NOT_USED unused;
    unused = snprintf(query_buf, 128,
                      "SELECT a, %s, CAST(%s as char) FROM test_datetime.dt1",
                      col_name, col_name);
    query = query_buf;

    sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"",
               drizzle_strerror(driz_ret), drizzle_error(con), query);
    driz_ret = drizzle_stmt_execute(sth);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
               drizzle_strerror(driz_ret), drizzle_error(con), query);
    driz_ret = drizzle_stmt_buffer(sth);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"",
               drizzle_strerror(driz_ret), drizzle_error(con), query);

    query = NULL;

    ASSERT_EQ(rows_in_table, drizzle_stmt_row_count(sth));
    cur_row = 0;
    while (drizzle_stmt_fetch(sth) != DRIZZLE_RETURN_ROW_END)
    {
      size_t server_strval_lth, drizzle_strval_lth;
      const char *server_strval, *drizzle_strval;

      cur_row++;

      unsigned columnA = drizzle_stmt_get_int(sth, 0, &driz_ret);
      ASSERT_EQ(driz_ret, DRIZZLE_RETURN_OK);

      drizzle_strval =
          drizzle_stmt_get_string(sth, 1, &drizzle_strval_lth, &driz_ret);
      ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Raw column '%s' of row %u",
                 col_name, columnA);
      ASSERT_EQ(strlen(drizzle_strval), drizzle_strval_lth);

      server_strval = drizzle_stmt_get_string(sth, 2, &server_strval_lth, &driz_ret);
      ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Cast column '%s' of row %u",
                 col_name, columnA);
      ASSERT_EQ(strlen(server_strval), server_strval_lth);

      printf("row=%u col=%s: '%s' / '%s'\n", columnA, col_name, drizzle_strval,
             server_strval);

      ASSERT_STREQ_(server_strval, drizzle_strval,
                    "Row %u, column '%s': server strval does not match libdrizzle strval",
                    columnA, col_name);
    }
    ASSERT_EQ(rows_in_table, cur_row);

    driz_ret = drizzle_stmt_close(sth);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s",
               drizzle_strerror(driz_ret), drizzle_error(con));
  }

  CHECKED_QUERY("DROP TABLE test_datetime.dt1");

  tear_down_schema("test_datetime");

  return EXIT_SUCCESS;
}
