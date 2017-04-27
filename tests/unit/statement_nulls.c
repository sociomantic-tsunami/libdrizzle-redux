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

#define _GNU_SOURCE

#include "tests/unit/common.h"
#include <yatl/lite.h>

#include <libdrizzle-5.1/libdrizzle.h>

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  drizzle_result_st *result;
  drizzle_return_t driz_ret;
  drizzle_stmt_st *stmt;

  set_up_connection();
  set_up_schema("test_stmt_nulls");

  CHECKED_QUERY("CREATE TABLE `t1` ("
                "`a` int(10) NOT NULL AUTO_INCREMENT,"
                "`b` varchar(255) NOT NULL,"
                "`c` varchar(16) NOT NULL,"
                "`d` varchar(32) DEFAULT NULL,"
                "`e` varchar(255) DEFAULT NULL,"
                "`f` varchar(255) DEFAULT NULL,"
                "`g` int(1) NOT NULL DEFAULT '1',"
                "`h` varchar(32) DEFAULT NULL,"
                "`i` int(2) DEFAULT NULL,"
                "`j` char(36) DEFAULT NULL,"
                "PRIMARY KEY (`a`)"
                ") ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8");

  CHECKED_QUERY("INSERT INTO `t1` (`b`, `c`, `e`, `f`, `g`, `h`, `j`)"
                "VALUES ('never', '1492672507', 'question', 'the',"
                "0, 'answer', 'd7f15abb-a482-43be-99c2-c175408c1bd9')");

  ASSERT_EQ(drizzle_result_affected_rows(result), 1);

  const char *query= "SELECT * FROM t1";
  stmt = drizzle_stmt_prepare(con, query, strlen(query), &driz_ret);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, preparing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);

  driz_ret = drizzle_stmt_execute(stmt);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, executing \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);
  driz_ret = drizzle_stmt_buffer(stmt);
  ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, buffering \"%s\"",
             drizzle_strerror(driz_ret), drizzle_error(con), query);

  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "drizzle_stmt_close() %s",
             drizzle_error(con));

  CHECKED_QUERY("DROP TABLE test_stmt_nulls.t1");

  tear_down_schema("test_stmt_nulls");

  return EXIT_SUCCESS;
}
