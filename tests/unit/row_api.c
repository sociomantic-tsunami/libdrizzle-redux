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
//#include "tests/unit/common.h"

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  //int num_fields;
  drizzle_result_st *result;
  drizzle_return_t ret;

  drizzle_st *con = drizzle_create(getenv("MYSQL_SERVER"),
                                   getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                                        : DRIZZLE_DEFAULT_TCP_PORT,
                                   getenv("MYSQL_USER"),
                                   getenv("MYSQL_PASSWORD"),
                                   getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_query(con, "DROP SCHEMA IF EXISTS test_row_api", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA test_row_api (%s)",
             drizzle_error(con));

  drizzle_query(con, "CREATE SCHEMA test_row_api", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA test_row_api (%s)",
             drizzle_error(con));

  ret = drizzle_select_db(con, "test_row_api");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE test_row_api");

  drizzle_query(con, "CREATE TABLE test_row_api.t1 (a INT, b INT)", 0, &ret);
  ASSERT_EQ(DRIZZLE_RETURN_OK, ret);

  drizzle_query(con, "INSERT INTO test_row_api.t1 VALUES (1,4),(2,5),(3,6)", 0, &ret);
  ASSERT_EQ(DRIZZLE_RETURN_OK, ret);

  result= drizzle_query(con, "SELECT * FROM test_row_api.t1", 0, &ret);
  ASSERT_EQ(DRIZZLE_RETURN_OK, ret);

  size_t offset;
  size_t size;
  size_t total;

  ret= drizzle_column_buffer(result);

  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("drizzle_column_skip:%s\n", drizzle_error(con));
    return 1;
  }

  ASSERT_EQ(0, drizzle_row_read(NULL, &ret));
  while (drizzle_row_read(result, &ret) != 0 && ret == DRIZZLE_RETURN_OK)
  {
    while (1)
    {
      drizzle_field_read(NULL, &offset, &size, &total, &ret);
      ASSERT_EQ(ret, DRIZZLE_RETURN_INVALID_ARGUMENT);
      drizzle_field_t field = drizzle_field_read(result, &offset, &size, &total, &ret);
      if (ret == DRIZZLE_RETURN_ROW_END)
      {
        break;
      }
      else if (ret != DRIZZLE_RETURN_OK)
      {
          printf("drizzle_field_read:%s\n", drizzle_error(con));
          return 1;
      }

      if (field == NULL)
      {
        printf("NULL");
      }
      else
      {
        printf("%.*s", (int)size, field);
      }

      if (offset + size == total)
      {
        printf("\t");
      }
    }
    printf("\n");
  }

  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("drizzle_row_read:%s\n", drizzle_error(con));
    return 1;
  }

  drizzle_result_free(result);

  drizzle_close(con);

  drizzle_query(con, "DROP TABLE test_row_api.t1", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP TABLE test_row_api.t1");

  drizzle_query(con, "DROP SCHEMA IF EXISTS test_row_api", 0, &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP SCHEMA test_row_api (%s) %s",
             drizzle_error(con), drizzle_strerror(ret));

  drizzle_close(con);
  //drizzle_free(drizzle);

  return EXIT_SUCCESS;
}
