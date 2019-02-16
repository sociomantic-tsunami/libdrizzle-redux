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
#include "tests/unit/common.h"

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_row_t row;
  int num_fields;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;

  set_up_connection();
  set_up_schema("test_row");

  CHECKED_QUERY("CREATE TABLE test_row.t1 (a INT)");

  CHECKED_QUERY("INSERT INTO test_row.t1 VALUES (1),(2),(3)");

  CHECKED_QUERY("SELECT * FROM test_row.t1");
  drizzle_result_buffer(result);
  num_fields = drizzle_result_column_count(result);

  if (num_fields != 1)
  {
    printf("Retrieved bad number of fields\n");
    return EXIT_FAILURE;
  }
  row = drizzle_row_next(result);
  if (row == NULL)
  {
    printf("Could not get the next row\n");
    return EXIT_FAILURE;
  }

  if (strcmp(row[0], "1") != 0)
  {
    printf("Retrieved bad next row value\n");
    return EXIT_FAILURE;
  }
  drizzle_row_seek(result, 3);
  row = drizzle_row_prev(result);
  if (row == NULL)
  {
    printf("Could not get prev row\n");
    return EXIT_FAILURE;
  }
  if (strcmp(row[0], "3") != 0)
  {
    printf("Retrieved bad prev row value: %s\n", row[0]);
    return EXIT_FAILURE;
  }
  row = drizzle_row_index(result, 1);
  if (row == NULL)
  {
    printf("Could not get indexed row\n");
    return EXIT_FAILURE;
  }
  if (strcmp(row[0], "2") != 0)
  {
    printf("Retrieved bad indexed row value: %s\n", row[0]);
    return EXIT_FAILURE;
  }
  if (drizzle_row_current(result) != 2)
  {
    printf("Index at wrong pos\n");
    return EXIT_FAILURE;
  }
  size_t *sizes = drizzle_row_field_sizes(result);
  ASSERT_EQ(sizes[0], 1);

  drizzle_result_free(result);

  CHECKED_QUERY("DROP TABLE test_row.t1");

  tear_down_schema("test_row");

  return EXIT_SUCCESS;
}
