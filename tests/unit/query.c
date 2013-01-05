/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012 Andrew Hutchings (andrew@linuxjedi.co.uk)
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
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_row_t row;
  int num_fields;

  drizzle_st *con= drizzle_create_tcp(getenv("MYSQL_SERVER"),
                                      getenv("MYSQL_PORT") ? atoi("MYSQL_PORT") : DRIZZLE_DEFAULT_TCP_PORT,
                                      getenv("MYSQL_USER"),
                                      getenv("MYSQL_PASSWORD"),
                                      getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  drizzle_return_t ret= drizzle_connect(con);
  if (ret == DRIZZLE_RETURN_COULD_NOT_CONNECT)
  {
    const char *error= drizzle_error(con);
    drizzle_quit(con);
    SKIP_IF_(ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", error, drizzle_strerror(ret));
  }
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "drizzle_connect(): %s(%s)", drizzle_error(con), drizzle_strerror(ret));

  drizzle_query_str(con, "DROP SCHEMA IF EXISTS libdrizzle", &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA libdrizzle (%s)", drizzle_error(con));

  drizzle_query_str(con, "CREATE SCHEMA libdrizzle", &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "CREATE SCHEMA libdrizzle (%s)", drizzle_error(con));

  ret= drizzle_select_db(con, "libdrizzle");
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "USE libdrizzle");

  drizzle_query_str(con, "create table libdrizzle.t1 (a int)", &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Create table failure\n");
    return EXIT_FAILURE;
  }

  drizzle_query_str(con, "insert into libdrizzle.t1 values (1),(2),(3)", &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Insert failure\n");
    return EXIT_FAILURE;
  }

  drizzle_result_st *result= drizzle_query_str(con, "select * from libdrizzle.t1", &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Select failure\n");
    return EXIT_FAILURE;
  }
  drizzle_result_buffer(result);
  num_fields= drizzle_result_column_count(result);

  if (num_fields != 1)
  {
    printf("Retrieved bad number of fields\n");
    return EXIT_FAILURE;
  }
  int i= 0;
  char buf[10];
  while ((row = drizzle_row_next(result)))
  {
    i++;
    snprintf(buf, 10, "%d", i);
    if (strcmp(row[0], buf) != 0)
    {
      printf("Retrieved bad row value\n");
      return EXIT_FAILURE;
    }
  }
  /* Should have had 3 rows */
  if (i != 3)
  {
    printf("Retrieved bad number of rows\n");
    return EXIT_FAILURE;
  }

  drizzle_result_free(result);

  drizzle_query_str(con, "DROP TABLE libdrizzle.t1", &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP TABLE libdrizzle.t1");

  drizzle_query_str(con, "DROP SCHEMA IF EXISTS libdrizzle", &ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "DROP SCHEMA libdrizzle (%s)", drizzle_error(con));

  ret= drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, ret, "%s", drizzle_strerror(ret));

  return EXIT_SUCCESS;
}
