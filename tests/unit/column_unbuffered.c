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

const char *column_names[4] = {
  "NULL", "a", "b", "c"
};

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  int num_fields;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;
  drizzle_column_st *column;

  set_up_connection();

  CHECKED_QUERY("SET CHARACTER SET latin1"); /* Or any other one-byte-per-character
                                                character set */
  set_up_schema("test_column");

  CHECKED_QUERY("CREATE TABLE test_column.t1 (a INT PRIMARY KEY AUTO_INCREMENT, b INT, c INT, d INT)");

  CHECKED_QUERY("INSERT INTO test_column.t1 (b,c,d) VALUES (1,2,3),(1,2,3),(1,2,3)");

  CHECKED_QUERY("SELECT * FROM test_column.t1");

  // check number of column in the result set
  ASSERT_EQ(0, drizzle_result_column_count(NULL));
  num_fields = drizzle_result_column_count(result);
  ASSERT_EQ_(num_fields, 4, "Retrieved bad number of fields");

  // check skipping a column in the result set
  ASSERT_EQ_(drizzle_column_skip(NULL), DRIZZLE_RETURN_INVALID_ARGUMENT,
    "Can't skip column for result=NULL");
  drizzle_column_skip(result);
  ASSERT_EQ(drizzle_column_current(result), 1);

  // check unbuffered reading of a column
  ASSERT_NULL_(drizzle_column_read(NULL, &driz_ret), "Couldn't read column");
  column = drizzle_column_read(result, &driz_ret);
  ASSERT_EQ_(drizzle_column_drizzle_result(column), result,
    "Wrong result set for read column");

  ASSERT_EQ(drizzle_column_current(NULL), 0);
  ASSERT_EQ(drizzle_column_current(result), 2);

  // seek to a specific column by index
  drizzle_column_seek(result, 0);

  ASSERT_EQ(drizzle_column_current(result), 0);

  // skip all column in a result set
  ASSERT_EQ(drizzle_column_skip_all(NULL), DRIZZLE_RETURN_INVALID_ARGUMENT);
  CHECK(drizzle_column_skip_all(result));

  ASSERT_EQ(drizzle_column_current(result), 3);

  drizzle_result_free(result);

  drizzle_close(con);

  CHECKED_QUERY("DROP TABLE test_column.t1");

  tear_down_schema("test_column");

  return EXIT_SUCCESS;
}
