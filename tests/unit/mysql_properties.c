/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright dunnhumby Germany GmbH 2019
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
  drizzle_result_st *result;
  drizzle_row_t row;
  drizzle_return_t driz_ret;

  set_up_connection();

  // Check MySQL server version string
  ASSERT_NULL_(drizzle_server_version(NULL), "con is NULL");
  result = drizzle_query(con, "SHOW variables like 'version'", 0, &driz_ret);
  drizzle_result_buffer(result);
  ASSERT_EQ(1, drizzle_result_row_count(result));
  row = drizzle_row_next(result);
  ASSERT_STREQ(row[1], drizzle_server_version(con));

  // Ping server
  result = drizzle_ping(con, &driz_ret);
  drizzle_result_buffer(result);
  ASSERT_EQ(DRIZZLE_RETURN_OK, driz_ret);

  // Check file descriptor for the connection
  ASSERT_EQ(-1, drizzle_fd(NULL));
  ASSERT_NEQ(-1, drizzle_fd(con));

  // check server capabilities
  ASSERT_EQ(DRIZZLE_CAPABILITIES_NONE, drizzle_capabilities(NULL));
  ASSERT_NEQ(DRIZZLE_CAPABILITIES_NONE, drizzle_capabilities(con));

  // check the status of the drizzle connection
  ASSERT_EQ(DRIZZLE_CON_STATUS_NONE, drizzle_status(NULL));
  ASSERT_NEQ(DRIZZLE_CON_STATUS_NONE, drizzle_status(con));

  // check the maximum size of a data packet sent by the server
  ASSERT_EQ(0, drizzle_max_packet_size(NULL));
  ASSERT_EQ(UINT32_MAX, drizzle_max_packet_size(con));

  // check the state of the MySQL
  ASSERT_NULL_( drizzle_sqlstate(NULL), "can't get sql state for con=NULL");
  driz_ret = drizzle_select_db(con, "invalid_database");
  ASSERT_STREQ("42000", drizzle_sqlstate(con));

  // Check server charset
  ASSERT_EQ(DRIZZLE_CHARSET_NONE, drizzle_charset(NULL));
  ASSERT_NEQ(DRIZZLE_CHARSET_NONE, drizzle_charset(con));

  // check protocol version
  ASSERT_EQ(0, drizzle_protocol_version(NULL));
  result = drizzle_query(con, "SHOW VARIABLES LIKE 'protocol_version'", 0, &driz_ret);
  drizzle_result_buffer(result);
  row = drizzle_row_next(result);
  uint32_t protocol_version = atoi(row[1]);
  ASSERT_EQ(protocol_version, drizzle_protocol_version(con));

  // check connection id
  result = drizzle_query(con, "SELECT CONNECTION_ID()", 0, &driz_ret);
  drizzle_result_buffer(result);
  row = drizzle_row_next(result);

  uint32_t conn_id = atoi(row[0]);
  result = drizzle_kill(con, conn_id, &driz_ret);
  ASSERT_EQ(0, drizzle_result_row_count(result));
  drizzle_result_free(result);

  close_connection_on_exit();
}
