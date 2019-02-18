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

#include <libdrizzle-redux/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECKED_QUERY(cmd) \
  drizzle_query(con, cmd, 0, &ret); \
  ASSERT_EQ_(ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", \
             drizzle_strerror(ret), drizzle_error(con), cmd);


struct warning_st
{
  int error_code;
  char error_msg[127];
};

typedef struct warning_st warning_st;

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_row_t row;
  int num_fields;

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

  CHECKED_QUERY("DROP SCHEMA IF EXISTS test_query");

  CHECKED_QUERY("CREATE SCHEMA test_query");

  ret = drizzle_select_db(con, "invalid_schema");
  ASSERT_EQ_(1049, drizzle_error_code(con), "Can't select invalid schema name");
  ret = drizzle_select_db(con, NULL);
  ASSERT_EQ(DRIZZLE_RETURN_INVALID_ARGUMENT, ret);
  ret = drizzle_select_db(con, "test_query");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE test_query");

  ASSERT_NULL_(
    drizzle_query(NULL, "CREATE TABLE test_query.t1 (a TINYINT NOT NULL, b CHAR(4))",
      0, &ret), "Can't query with con=NULL");
  ASSERT_EQ(ret, DRIZZLE_RETURN_INVALID_ARGUMENT);

  drizzle_query(con, "CREATE TABLE test_query.t1 (a TINYINT NOT NULL, b CHAR(4))", 0, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Create table failure\n");
    return EXIT_FAILURE;
  }

  drizzle_query(con, "SET SESSION sql_mode = 'NO_AUTO_CREATE_USER,NO_ENGINE_SUBSTITUTION'", 0, &ret);
  drizzle_result_st *result= drizzle_query(con,
    "INSERT INTO test_query.t1 VALUES (10, 'mysql'),(NULL, 'test'),(300, 'xyz')",
    0, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Insert failure %s %d\n", drizzle_strerror(ret), drizzle_error_code(con));
    return EXIT_FAILURE;
  }

  ASSERT_EQ(0, drizzle_result_warning_count(NULL));
  ASSERT_EQ(3, drizzle_result_warning_count(result));

  result = drizzle_query(con, "SHOW WARNINGS", 0, &ret);
  ASSERT_EQ(DRIZZLE_RETURN_OK, ret);

  drizzle_result_buffer(result);
  ASSERT_EQ(0, drizzle_result_row_count(NULL));
  ASSERT_EQ(3, drizzle_result_row_count(result));

  const warning_st warnings[3] =
  {
    (struct warning_st){1265, "Data truncated for column 'b' at row 1"},
    (struct warning_st){1048, "Column 'a' cannot be null"},
    (struct warning_st){1264, "Out of range value for column 'a' at row 3"}
  };

  for (uint64_t i = 0; i < 3; i++)
  {
    row = drizzle_row_index(result, i);
    ASSERT_EQ(warnings[i].error_code, atoi(row[1]));
    ASSERT_TRUE(strcmp(row[2], warnings[i].error_msg) == 0);
  }


  //warning_st a = (struct warning_st){1265, "a"};
  //warnings[0] = a;
   /*=
  {
    warning_st(1265, "a"),
    warning_st(1265, "a"),
    warning_st(1265, "a")
  };*/

  // https://dev.mysql.com/doc/refman/5.7/en/show-warnings.html
  // test warnings
  result = drizzle_query(con, "SELECT * FROM test_query.no_such_table", 0, &ret);
  ASSERT_EQ(1146, drizzle_error_code(con));
  ASSERT_NULL_(drizzle_result_sqlstate(NULL), "No sqlstate for a NULL result");
  ASSERT_STREQ("42S02", drizzle_result_sqlstate(result));
  ASSERT_EQ(0, drizzle_result_error_code(NULL));
  ASSERT_EQ(1146, drizzle_result_error_code(result));
  drizzle_result_free(result);

  result = drizzle_query(con, "select * from test_query.t1", 0, &ret);

  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Select failure\n");
    return EXIT_FAILURE;
  }

  ASSERT_NULL_(drizzle_result_drizzle_con(NULL), "Result object is NULL");
  ASSERT_EQ(con, drizzle_result_drizzle_con(result));
  drizzle_result_buffer(result);
  num_fields = drizzle_result_column_count(result);

  if (num_fields != 2)
  {
    printf("Retrieved bad number of fields\n");
    return EXIT_FAILURE;
  }
  int i = 0;
  char buf[10];
  int expected[] = {10, 0, 127};
  while ((row = drizzle_row_next(result)))
  {
    snprintf(buf, 10, "%d", expected[i]);
    if (strcmp(row[0], buf) != 0)
    {
      printf("Retrieved bad row value\n");
      return EXIT_FAILURE;
    }
    i++;
  }

  /* Should have had 3 rows */
  if (drizzle_row_current(result) != 3)
  {
    printf("Retrieved bad number of rows\n");
    return EXIT_FAILURE;
  }

  drizzle_result_free_all(NULL);
  drizzle_row_seek(result, 1);
  drizzle_result_free(result);
  drizzle_close(con);

  drizzle_query(con, "DROP TABLE test_query.t1", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP TABLE test_query.t1");

  drizzle_query(con, "DROP SCHEMA IF EXISTS test_query", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP SCHEMA test_query (%s)",
             drizzle_error(con));

  ret = drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_strerror(ret));

  return EXIT_SUCCESS;
}
