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
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_st *con;
  drizzle_return_t ret;
  drizzle_result_st *result;
  drizzle_row_t row;
  int num_fields;

  con = drizzle_create_tcp("localhost", 3306, "root", "", "libdrizzle", 0);
  if (con == NULL)
  {
    printf("Drizzle connection object creation error\n");
    return EXIT_FAILURE;
  }
  ret = drizzle_connect(con);
  if (ret != DRIZZLE_RETURN_OK)
  {
    drizzle_quit(con);
    SKIP_IF_(ret != DRIZZLE_RETURN_OK, "Drizzle connection failure");
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

  result= drizzle_query_str(con, "select * from libdrizzle.t1", &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Select failure\n");
    return EXIT_FAILURE;
  }

  if (drizzle_column_buffer(result) != DRIZZLE_RETURN_OK)
  {
    printf("Column buffer failure\n");
    return EXIT_FAILURE;
  }
  num_fields= drizzle_result_column_count(result);

  if (num_fields != 1)
  {
    printf("Retrieved bad number of fields\n");
    return EXIT_FAILURE;
  }

  int i= 0;
  char buf[10];
  while(1)
  {
    row= drizzle_row_buffer(result, &ret);
    if (ret != DRIZZLE_RETURN_OK)
    {
      printf("Row retrieval error\n");
      break;
    }
    if (row == NULL)
    {
      // EOF
      break;
    }
    i++;
    snprintf(buf, 10, "%d", i);
    if (strcmp(row[0], buf) != 0)
    {
      printf("Retrieved bad row data\n");
      return EXIT_FAILURE;
    }
    drizzle_row_free(result, row);
  }
  /* we should have had 3 rows */
  if (i != 3)
  {
    printf("Retrieved bad number of rows\n");
    return EXIT_FAILURE;
  }

  drizzle_result_free(result);

  drizzle_query_str(con, "drop table libdrizzle.t1", &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drop table failure\n");
    return EXIT_FAILURE;
  }


  drizzle_quit(con);
  return EXIT_SUCCESS;
}
