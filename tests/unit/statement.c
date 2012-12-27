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

#include <libdrizzle-5.1/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <inttypes.h>
#include <string.h>

#ifndef EXIT_SKIP
# define EXIT_SKIP 77
#endif

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_st *con;
  drizzle_return_t ret;
  drizzle_stmt_st *stmt;

  con = drizzle_create_tcp("localhost", 3306, "root", "", "libdrizzle", 0);
  if (con == NULL)
  {
    printf("Drizzle connection object creation error\n");
    return EXIT_FAILURE;
  }
  ret = drizzle_connect(con);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drizzle connection failure\n");
    drizzle_quit(con);
    return EXIT_SKIP;
  }

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

  const char *query= "select * from libdrizzle.t1 where a > ?";
  stmt= drizzle_stmt_prepare(con, query, strlen(query), &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Prepare failure\n");
    return EXIT_FAILURE;
  }
  /* Query should have 1 param */
  if (drizzle_stmt_param_count(stmt) != 1)
  {
    printf("Retrieved bad param count\n");
    return EXIT_FAILURE;
  }

  uint32_t val= 1;
  ret = drizzle_stmt_bind_param(stmt, 0, DRIZZLE_COLUMN_TYPE_LONG, &val, 4, DRIZZLE_BIND_OPTION_NONE);
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
  /* Result should have 2 rows */
  if (drizzle_stmt_row_count(stmt) != 2)
  {
    printf("Retrieved bad row count\n");
    return EXIT_FAILURE;
  }
  uint32_t i= 1;
  while (drizzle_stmt_fetch(stmt) != DRIZZLE_RETURN_ROW_END)
  {
    uint32_t *res_val;
    res_val= (uint32_t*)drizzle_stmt_item_data(stmt, 0);
    i++;
    if (*res_val != i)
    {
      printf("Retrieved unexpected value\n");
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
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Statement close failure ret: %d, err: %d, msg: %s\n", ret, drizzle_errno(con), drizzle_error(con));
    return EXIT_FAILURE;
  }

  drizzle_query_str(con, "drop table libdrizzle.t1", &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drop table failure\n");
    return EXIT_FAILURE;
  }


  drizzle_quit(con);
  return EXIT_SUCCESS;
}
