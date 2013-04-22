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

/*
 * This test inserts a set of rows with NULLs in various columns, and verifies
 * that returned rows have NULLs in the proper columns (and non-NULL values in
 * the other columns). A binary count is used to generate 64 combinations of
 * NULL and non-NULL across 6 columns.
 *
 * Both the insert and the query are performed twice, once using the text-based
 * query interface and once using the prepared-statement binary-based query interface.
 */

#include <yatl/lite.h>

#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHECKED_QUERY(cmd) result = drizzle_query(con, cmd, 0, &driz_ret); ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), cmd);

#define TEST_PREPARED_STATEMENTS

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_row_t row;
  int num_fields;
  drizzle_result_st *result;
  int table_size;
#ifdef TEST_PREPARED_STATEMENTS
  drizzle_stmt_st *sth;
#endif
  
  drizzle_st *con= drizzle_create(getenv("MYSQL_SERVER"),
                                  getenv("MYSQL_PORT") ? atoi("MYSQL_PORT") : DRIZZLE_DEFAULT_TCP_PORT,
                                  getenv("MYSQL_USER"),
                                  getenv("MYSQL_PASSWORD"),
                                  getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");
  
  drizzle_return_t driz_ret= drizzle_connect(con);
  if (driz_ret == DRIZZLE_RETURN_COULD_NOT_CONNECT)
  {
    char error[DRIZZLE_MAX_ERROR_SIZE];
    strncpy(error, drizzle_error(con), DRIZZLE_MAX_ERROR_SIZE);
    drizzle_quit(con);
    SKIP_IF_(driz_ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", error, drizzle_strerror(driz_ret));
  }
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "drizzle_connect(): %s(%s)", drizzle_error(con), drizzle_strerror(driz_ret));
  
  CHECKED_QUERY("DROP SCHEMA IF EXISTS libdrizzle");
  
  CHECKED_QUERY("CREATE SCHEMA libdrizzle");
  
  driz_ret= drizzle_select_db(con, "libdrizzle");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "USE libdrizzle");
  
  CHECKED_QUERY("create table libdrizzle.t1 (a int, b int, c int, d int, e int, f int, g int, h int, i int, j int, k int)");
  
#define NCOLS 11
  
  char *querybuf = calloc(256 + 60*64, 1);
  strcpy(querybuf, "insert into libdrizzle.t1 values ");
  char *p = querybuf + strlen(querybuf);
  
  for(int sym = 0; sym < 64; sym++) {
    int cn = 0;
    *p++ = '(';
#define APPENDMAYBE(b)  if (sym & b) { strcpy(p, "NULL"); p += 4; } else { p += sprintf(p, "%d", sym*NCOLS + cn); }
    APPENDMAYBE(0x01); *p++ = ','; cn++;
    APPENDMAYBE(0x02); *p++ = ','; cn++;
    APPENDMAYBE(0x04);             cn++;
    p += sprintf(p, ",%d,%d,%d,%d,", sym*NCOLS + 3, sym*NCOLS + 4, sym*NCOLS + 5, sym*NCOLS + 6);
    cn += 4;
    APPENDMAYBE(0x08);             cn++;
    p += sprintf(p, ",%d,", sym*NCOLS + 8); cn++;
    APPENDMAYBE(0x10); *p++ = ','; cn++;
    APPENDMAYBE(0x20);             cn++;
    *p++ = ')';
    if (sym < 63)
      *p++ = ',';
#undef APPENDMAYBE
  }
  CHECKED_QUERY(querybuf);
  
#ifdef TEST_PREPARED_STATEMENTS
  strcpy(querybuf, "insert into libdrizzle.t1 values (?,?,?,?,?,?,?,?,?,?,?)");
  sth = drizzle_stmt_prepare(con, querybuf, strlen(querybuf), &driz_ret);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), querybuf);
  
  for(int sym = 0; sym < 64; sym++) {
    // driz_ret= drizzle_stmt_reset(sth);
    // ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, resetting statement", drizzle_strerror(driz_ret), drizzle_error(con));
    
#define SETMAYBE(cn,b) if (sym & b) driz_ret = drizzle_stmt_set_null(sth, cn); else SETALWAYS(cn);
#define SETALWAYS(cn) driz_ret = drizzle_stmt_set_short(sth, cn, sym*NCOLS + cn + 1000, 0)
    ASSERT_EQ(driz_ret, DRIZZLE_RETURN_OK);
    SETMAYBE(0,0x01);
    SETMAYBE(1,0x02);
    SETMAYBE(2,0x04);
    SETALWAYS(3);
    SETALWAYS(4);
    SETALWAYS(5);
    SETALWAYS(6);
    SETMAYBE(7,0x08);
    SETALWAYS(8);
    SETMAYBE(9,0x10);
    SETMAYBE(10,0x20);
#undef SETMAYBE
    
    driz_ret = drizzle_stmt_execute(sth);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing insert, sym=%d", drizzle_strerror(driz_ret), drizzle_error(con), sym);
    
    driz_ret = drizzle_stmt_buffer(sth);
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering result, sym=%d", drizzle_strerror(driz_ret), drizzle_error(con), sym);
    
    // ASSERT_EQ(drizzle_stmt_affected_rows, 1);
  }
  
  ASSERT_EQ(drizzle_stmt_close(sth), DRIZZLE_RETURN_OK);
  
  table_size = 128;
#else
  table_size = 64;
#endif
  
  CHECKED_QUERY("select a,b,c,d,e,f,g,h,i,j,k from libdrizzle.t1 order by e");
  drizzle_result_buffer(result);
  num_fields= drizzle_result_column_count(result);
  ASSERT_EQ_(num_fields, NCOLS, "Bad number of fields, expected %d, got %d", 11, num_fields);
  
  int cur_row = 0;
  char nbuf[16];
  while ((row = drizzle_row_next(result))) {
    cur_row ++;
    int sym, rowbase;
    
    /* 'sym' is the value used to decide which fields have NULLs or not.
     * 'rowbase' is the number that would be stored in the first field of this
     * row (assuming it's non-NULL).
     */
    
    if (cur_row <= 64) {
      sym = cur_row - 1;
      rowbase = sym*NCOLS;
    } else {
      sym = cur_row - 65;
      rowbase = 1000 + sym*NCOLS;
    }
    
#define NULLMAYBE(cn, b)  if (sym & b) { ASSERT_NULL_(row[cn], "Column %d, row %d should be NULL", cn+1, cur_row); } else { ASSERT_NOT_NULL_(row[cn], "Column %d, row %d should not be NULL", cn+1, cur_row); sprintf(nbuf, "%d", rowbase+cn); ASSERT_STREQ(row[cn], nbuf); }
#define NULLNEVER(cn) NULLMAYBE(cn, 0)
    
    NULLMAYBE(0, 0x01);
    NULLMAYBE(1, 0x02);
    NULLMAYBE(2, 0x04);
    NULLNEVER(3);
    NULLNEVER(4);
    NULLNEVER(5);
    NULLNEVER(6);
    NULLMAYBE(7, 0x08);
    NULLNEVER(8);
    NULLMAYBE(9, 0x10);
    NULLMAYBE(10, 0x20);
    
#undef NULLMAYBE
#undef NULLNEVER
  }
  ASSERT_EQ(cur_row, table_size);
  
  drizzle_result_free(result);
  
#ifdef TEST_PREPARED_STATEMENTS
  strcpy(querybuf, "select a,b,c,d,e,f,g,h,i,j,k from libdrizzle.t1 order by e");
  sth = drizzle_stmt_prepare(con, querybuf, strlen(querybuf), &driz_ret);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), querybuf);
  driz_ret = drizzle_stmt_execute(sth);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), querybuf);
  
  cur_row = 0;
  for (;;) {
    driz_ret = drizzle_stmt_fetch(sth);
    if (driz_ret == DRIZZLE_RETURN_ROW_END)
      break;
    cur_row ++;
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, fetching row #%d", drizzle_strerror(driz_ret), drizzle_error(con), cur_row);
    
    /* 'sym' is the value used to decide which fields have NULLs or not.
     * 'rowbase' is the number that would be stored in the first field of this
     * row (assuming it's non-NULL).
     */
    
    int sym, rowbase;
    
    if (cur_row <= 64) {
      sym = cur_row - 1;
      rowbase = sym*NCOLS;
    } else {
      sym = cur_row - 65;
      rowbase = 1000 + sym*NCOLS;
    }
    
    bool isNull;
    uint32_t rowvalue;
    
#define GETNULLNESS(cn) isNull = drizzle_stmt_get_is_null(sth, cn, &driz_ret); ASSERT_EQ(driz_ret, DRIZZLE_RETURN_OK);
#define NULLNOTNOW(cn) ASSERT_FALSE_(isNull, "Column %d, row %d should not be NULL", cn+1, cur_row); rowvalue = drizzle_stmt_get_int(sth, cn, &driz_ret); ASSERT_EQ(driz_ret,DRIZZLE_RETURN_OK); ASSERT_EQ_(rowvalue, (unsigned)(rowbase + cn), "Column %d, row %d has unexpected data, expected: %d, got: %d", cn+1, cur_row, rowbase+cn, rowvalue);
#define NULLMAYBE(cn, b) GETNULLNESS(cn); if (sym & b) { ASSERT_TRUE_(isNull, "Column %d, row %d should be NULL", cn+1, cur_row); } else { NULLNOTNOW(cn); }
#define NULLNEVER(cn) GETNULLNESS(cn); NULLNOTNOW(cn);
    
    NULLMAYBE(0, 0x01);
    NULLMAYBE(1, 0x02);
    NULLMAYBE(2, 0x04);
    NULLNEVER(3);
    NULLNEVER(4);
    NULLNEVER(5);
    NULLNEVER(6);
    NULLMAYBE(7, 0x08);
    NULLNEVER(8);
    NULLMAYBE(9, 0x10);
    NULLMAYBE(10, 0x20);
    
#undef NULLMAYBE
#undef NULLNEVER
#undef GETNULLNESS
#undef NULLNOTNOW
  }
  ASSERT_EQ_(cur_row, table_size, "Current row not equal to table size, expected %d, got %d", table_size, cur_row);
  ASSERT_EQ(drizzle_stmt_close(sth), DRIZZLE_RETURN_OK);
#endif
  
  CHECKED_QUERY("DROP TABLE libdrizzle.t1");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "DROP TABLE libdrizzle.t1");
   
  CHECKED_QUERY("DROP SCHEMA IF EXISTS libdrizzle");

  driz_ret= drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s", drizzle_strerror(driz_ret));
 
  free(querybuf); 
  return EXIT_SUCCESS;
}
