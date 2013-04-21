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
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>

/* We've chosen the floats so that the are exactly representable in a normal binary floating-point format, so we should get exact results out. However, some compilers (pretty reasonably) warn when doing exact equality tests of floats. This gets around that. */
#define FLOAT_EQ_EXACT(a, b) ( isfinite(a) && isfinite(b) && !(a < b) && !(a > b) )

#define ASSERT_FLOATEQEXACT(a, b) ASSERT_TRUE_(FLOAT_EQ_EXACT(a, b), "Floating-point values %f != %f (difference is %g)", (a), (b), (a)-(b))

static const drizzle_column_type_t expected_column_types[9] = {
    DRIZZLE_COLUMN_TYPE_NONE, /* Columns are 1-indexed */
    DRIZZLE_COLUMN_TYPE_LONG,
    DRIZZLE_COLUMN_TYPE_TINY,
    DRIZZLE_COLUMN_TYPE_SHORT,
    DRIZZLE_COLUMN_TYPE_INT24,
    DRIZZLE_COLUMN_TYPE_LONG,
    DRIZZLE_COLUMN_TYPE_LONGLONG,
    DRIZZLE_COLUMN_TYPE_FLOAT,
    DRIZZLE_COLUMN_TYPE_DOUBLE
};

/* These are close to the maximum value for each integer type. A few bits are cleared in the middle of each value to detect other kinds of scrambling (e.g. inappropriate sign-extension of intermediate results). */
static const intmax_t column_maxes[5] = {
    0x7F,                /*                 127 */
    0x7FAF,              /*               32687 */
    0x7FFEFF,            /*             8388351 */
    0x7FFDFFFF,          /*          2147352575 */
    0x7FFBFFFFFFFFCFFF   /* 9222246136947920895 */
};

static const char *column_names[9] = {
    NULL,
    "a", "b", "c", "d", "e", "f", "g", "h"
};

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;
  drizzle_row_t row;
  const char *query;
  drizzle_stmt_st *sth;
  int num_fields;
  
  set_up_connection();
  set_up_schema();
  
  CHECKED_QUERY("create table libdrizzle.t1 (a int primary key auto_increment, b tinyint, c smallint, d mediumint, e int, f bigint, g float, h double)");
  
  /* Insert rows with pk 1 and 2 */
  CHECKED_QUERY("insert into libdrizzle.t1 (b,c,d,e,f,g,h) values (1,1,1,1,1,1,1), (127,32687,8388351,2147352575,9222246136947920895,443664,291.2711110711098);");
  ASSERT_EQ(drizzle_result_affected_rows(result), 2);
  
  /* Insert row with pk 3 */
  CHECKED_QUERY("insert into libdrizzle.t1 (b,c,d,e,f,g,h) ( select 0-b, 0-c, 0-d, 0-e, 0-f, g+1.015625, h+1 from t1 where a = 2 );");
  ASSERT_EQ(drizzle_result_affected_rows(result), 1);
  
  /* Insert row with pk 4 - test marshaling values we transmit */
  query = "insert into libdrizzle.t1 (b,c,d,e,f,g,h) values (?,?,?,?,?,?,?)";
  sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), query);
  CHECK(drizzle_stmt_set_tiny(sth,   0, 127, 0));
  CHECK(drizzle_stmt_set_short(sth,  1, 32687, 0));
  CHECK(drizzle_stmt_set_int(sth,    2, 8388351, 0));
  CHECK(drizzle_stmt_set_int(sth,    3, 2147352575, 0));
  CHECK(drizzle_stmt_set_bigint(sth, 4, 9222246136947920895, 0));
  CHECK(drizzle_stmt_set_float(sth,  5, 443664.0));
  CHECK(drizzle_stmt_set_double(sth, 6, 291.2711110711098));
  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), query);
  CHECK(drizzle_stmt_close(sth));
  
  /* TODO: Also send some negative values of each type */
  
  /* Read the table back, with values sent over the wire in text form */
  CHECKED_QUERY("select * from libdrizzle.t1 order by b, a");
  
  drizzle_result_buffer(result);
  num_fields= drizzle_result_column_count(result);
  
  ASSERT_EQ_(num_fields, 8, "Retrieved bad number of fields");
  
  int readback_order[4] = { 3, 1, 2, 4 };
  
  unsigned int cur_row= 0;
  drizzle_column_st *column;
  while ((row = drizzle_row_next(result)))
  {
    drizzle_column_seek(result, 0);
    int cur_column= 0;
    cur_row++;
    ASSERT_EQ(drizzle_row_current(result), cur_row);
    ASSERT_TRUE(cur_row <= 4);
    
    char expected_colA[10];
    sprintf(expected_colA, "%d", readback_order[cur_row-1]);
    ASSERT_EQ_(strcmp(row[0], expected_colA), 0, "Retrieved bad row value; row=%d got=%s expected=%s", cur_row, row[0], expected_colA);
    
    while ((column= drizzle_column_next(result)))
    {
      cur_column++;
      ASSERT_EQ_(strcmp(drizzle_column_db(column), "libdrizzle"), 0, "Column has bad DB name");
      ASSERT_EQ_(strcmp(drizzle_column_table(column), "t1"), 0, "Column had bad table name");
      ASSERT_EQ(drizzle_column_current(result), cur_column);
      ASSERT_STREQ(drizzle_column_name(column), column_names[cur_column]);
      ASSERT_EQ_(drizzle_column_type(column), expected_column_types[cur_column], "Column %d has type=%d expected=%d", cur_column, drizzle_column_type(column), expected_column_types[cur_column]);
    }
    ASSERT_EQ_(cur_column, 8, "Wrong column count");
    
    for (cur_column = 2; cur_column <= 6; cur_column ++) {
      if (cur_row == 2) {
        ASSERT_STREQ("1", row[cur_column-1]);
      } else if (cur_row == 3 || cur_row == 4) {
        char buf[25];
        snprintf(buf, 25, "%"PRIdMAX, column_maxes[cur_column-2]);
        ASSERT_STREQ(buf, row[cur_column-1]);
      } else if (cur_row == 1) {
        char buf[25];
        snprintf(buf, 25, "-%"PRIdMAX, column_maxes[cur_column-2]);
        ASSERT_STREQ(buf, row[cur_column-1]);
      }
    }
    
    if (cur_row == 1) {
      ASSERT_STREQ("443665", row[6]);
      ASSERT_STREQ("292.2711110711098", row[7]);
    }
    
    if (cur_row == 3 || cur_row == 4) {
      ASSERT_STREQ("443664", row[6]);
      ASSERT_STREQ("291.2711110711098", row[7]);
    }
    
    
  }
  /* Should have had 4 rows */
  ASSERT_EQ_(cur_row, 4, "Retrieved bad number of rows");
  
  drizzle_result_free(result);
  
  /* Read the table back, with values sent over the wire in binary form */
  query = "select a,b,c,d,e,f,g,h from libdrizzle.t1";
  sth = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), query);
  
  ASSERT_EQ(4, drizzle_stmt_row_count(sth));
  cur_row = 0;
  while (drizzle_stmt_fetch(sth) != DRIZZLE_RETURN_ROW_END)
  {
    cur_row ++;
    printf("Row %d\n", cur_row);
    
    int columnA = drizzle_stmt_get_int(sth, 0, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), 1, cur_row);
    
    int cur_column;
    intmax_t col_val, expect_val;
    char expect_strval[64];
    const char *col_strval;
    double col_dblval;
    size_t lth;
    
    for (cur_column = 2; cur_column <= 6; cur_column ++) {
      switch (columnA) {
        case 3:
          expect_val = ( 0 - column_maxes[cur_column-2] );
          break;
        case 1:
          expect_val = 1;
          break;
        case 2:
        case 4:
          expect_val = column_maxes[cur_column-2];
          break;
        default:
          ASSERT_FALSE_(1, "Row %d has column.a = %u", cur_row, columnA);
      }
      
      /* These columns are 0-based */
      
      col_val = drizzle_stmt_get_bigint(sth, cur_column-1, &driz_ret);
      ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
      col_strval = drizzle_stmt_get_string(sth, cur_column-1, &lth, &driz_ret);
      ASSERT_EQ(driz_ret, DRIZZLE_RETURN_OK);
      ASSERT_EQ(lth, strlen(col_strval));
      col_dblval = drizzle_stmt_get_double(sth, cur_column-1, &driz_ret);
      ASSERT_TRUE_(driz_ret == DRIZZLE_RETURN_OK || driz_ret == DRIZZLE_RETURN_TRUNCATED, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
      
      printf("  Column %d: %"PRIdMAX"  \"%s\"   %f\n", cur_column, col_val, col_strval, col_dblval);
      
      if (columnA == 3)
        continue;
      
      ASSERT_EQ_(expect_val, col_val, "Column %d of row %d: expected %"PRIdMAX", got %"PRIdMAX,
                 cur_column, cur_row, expect_val, col_val);
      if (cur_column <= 5) {
        uint32_t col_narrowval = drizzle_stmt_get_int(sth, cur_column-1, &driz_ret);
        ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
        ASSERT_EQ_(col_val, col_narrowval, "column %d of row %d: get_bigint returns %"PRIdMAX", get_int returns %"PRId32,
                   cur_column, cur_row, col_val, col_narrowval);
      }
      ASSERT_FLOATEQEXACT((double)expect_val, col_dblval);
      
      snprintf(expect_strval, 64, "%"PRIdMAX, expect_val);
      ASSERT_STREQ(expect_strval, col_strval);
    }
    
    float expect_floatval = 0.0;
    double expect_dblval = 0.0;
    switch (columnA) {
      case 1:
        expect_floatval = 1.0;
        expect_dblval = 1.0;
        break;
      case 2:
      case 4:
        expect_floatval = 443664.0;
        expect_dblval = 291.2711110711098;
        break;
      case 3:
        expect_floatval = 443665.0;
        expect_dblval = 292.2711110711098;
        break;
    }
    
    col_val = drizzle_stmt_get_int(sth, 6, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_TRUNCATED, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
    ASSERT_FLOATEQEXACT((float)col_val, trunc(expect_floatval));
    
    col_val = drizzle_stmt_get_bigint(sth, 6, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_TRUNCATED, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
    ASSERT_FLOATEQEXACT((float)col_val, trunc(expect_floatval));
    
    col_dblval = drizzle_stmt_get_double(sth, 6, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
    ASSERT_FLOATEQEXACT(col_dblval, expect_floatval);
    
    col_strval = drizzle_stmt_get_string(sth, 6, &lth, &driz_ret);
    ASSERT_EQ(driz_ret, DRIZZLE_RETURN_OK);
    
    printf("  Column %d: %"PRIdMAX"  \"%s\"   %f\n", 7, col_val, col_strval, col_dblval);
    
    
    
    col_val = drizzle_stmt_get_int(sth, 7, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_TRUNCATED, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
    ASSERT_FLOATEQEXACT((double)col_val, trunc(expect_dblval));
    
    col_val = drizzle_stmt_get_bigint(sth, 7, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_TRUNCATED, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
    ASSERT_FLOATEQEXACT((double)col_val, trunc(expect_dblval));
    
    col_dblval = drizzle_stmt_get_double(sth, 7, &driz_ret);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, column %d of row %d", drizzle_strerror(driz_ret), drizzle_error(con), cur_column, cur_row);
    ASSERT_FLOATEQEXACT(col_dblval, expect_dblval);
    
    col_strval = drizzle_stmt_get_string(sth, 7, &lth, &driz_ret);
    ASSERT_EQ(driz_ret, DRIZZLE_RETURN_OK);
    
    printf("  Column %d: %"PRIdMAX"  \"%s\"   %f\n", 8, col_val, col_strval, col_dblval);
  }
  
  ASSERT_EQ(cur_row, 4);
  
  driz_ret = drizzle_stmt_close(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s", drizzle_strerror(driz_ret), drizzle_error(con));
  
  drizzle_query(con, "DROP TABLE libdrizzle.t1", 0, &driz_ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "DROP TABLE libdrizzle.t1");
  
  tear_down_schema();
  
  return EXIT_SUCCESS;
}
