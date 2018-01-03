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

#include "tests/unit/common.c"
#include <libdrizzle-redux/libdrizzle.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern void log_fn_callback(const char *file, uint line, const char *func,
  const char *msg, drizzle_verbose_t verbose, void *context);
extern void log_fn_callback(const char *file, uint line, const char *func,
  const char *msg, drizzle_verbose_t verbose, void *context)
{

  void VARIABLE_IS_NOT_USED *cxt = context;

  printf("%-6s[%s:%d] : %s %s%s\n", drizzle_verbose_name(verbose), file, line, func,
      strlen(msg) > 0 ? "- " : "" , msg);
}


drizzle_return_t driz_ret;

/**
 * Test connection errors
 *
 * Creates a drizzle struct with the specified connection settings. Performs a
 * call to drizzle_connect and afterwards tests the expected return value and
 * error string with the actual values

 * @param[in]  host            host
 * @param[in]  port            port
 * @param[in]  user            user
 * @param[in]  password        password
 * @param[in]  db              database
 * @param[in]  ret_expected    expected return value
 * @param[in]  timeout         connection timeout
 */
void test_connection_error(const char *host, in_port_t port,
                           const char *user, const char *password,
                           const char *db, drizzle_return_t ret_expected,
                           int timeout);

void test_connection_error(const char *host, in_port_t port,
                           const char *user, const char *password,
                           const char *db, drizzle_return_t ret_expected,
                           int timeout)
{
  con = drizzle_create(host, port, user, password, db, NULL);
  drizzle_set_timeout(con, timeout);
  driz_ret = drizzle_connect(con);
  ASSERT_EQ(driz_ret, ret_expected);
  drizzle_quit(con);
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;

  // connection parameters
  const char *host = getenv("MYSQL_SERVER");
  in_port_t port = getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                        : DRIZZLE_DEFAULT_TCP_PORT;
  const char *user = getenv("MYSQL_USER");
  const char *pass = getenv("MYSQL_PASSWORD");
  const char *db = getenv("MYSQL_SCHEMA");

  // invalid host
  test_connection_error("1.2.3.4", port, "valid_user", "valid_pass", "valid_db",
    DRIZZLE_RETURN_TIMEOUT, 3);

  // invalid port
  test_connection_error(host, 1234, "valid_user", "valid_pass",
    "valid_db", DRIZZLE_RETURN_COULD_NOT_CONNECT, -1);

  // invalid user
  test_connection_error(host, port, "invalid_user", "valid_pass",
    "valid_db", DRIZZLE_RETURN_HANDSHAKE_FAILED, -1);

  // invalid pass
  test_connection_error(host, port, "valid_user", "invalid_pass",
    "valid_db", DRIZZLE_RETURN_HANDSHAKE_FAILED, -1);

  // invalid schema
  test_connection_error(host, port, user, pass, "invalid_db",
    DRIZZLE_RETURN_HANDSHAKE_FAILED, -1);

  opts = drizzle_options_create();
  drizzle_socket_set_options(opts, 10, 5, 3, 3);

  con = drizzle_create(host, port, user, pass, db, opts);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  int opt_val = drizzle_socket_get_option(con, DRIZZLE_SOCKET_OPTION_TIMEOUT);
  ASSERT_EQ_(10, opt_val, "unexpected value for socket option TIMEOUT: %d != 10",
    opt_val);

  opt_val = drizzle_socket_get_option(con, DRIZZLE_SOCKET_OPTION_KEEPIDLE);
  ASSERT_EQ_(5, opt_val, "unexpected value for socket option KEEPIDLE: %d != 5",
    opt_val);

  opt_val = drizzle_socket_get_option(con, DRIZZLE_SOCKET_OPTION_KEEPCNT);
  ASSERT_EQ_(3, opt_val, "unexpected value for socket option KEEPCNT: %d != 3",
    opt_val);

  opt_val = drizzle_socket_get_option(con, DRIZZLE_SOCKET_OPTION_KEEPINTVL);
  ASSERT_EQ_(3, opt_val, "unexpected value for socket option KEEPINTVL: %d != 3",
    opt_val);

  drizzle_socket_set_option(con, DRIZZLE_SOCKET_OPTION_TIMEOUT, 20);
  opt_val = drizzle_socket_get_option(con, DRIZZLE_SOCKET_OPTION_TIMEOUT);
  ASSERT_EQ_(20, opt_val, "unexpected value for socket option KEEPALIVE: %d != 20",
    opt_val);
  int cxt = 1;
  drizzle_set_log_fn(con, log_fn_callback, (void*)&cxt);
  drizzle_set_verbose(con, DRIZZLE_VERBOSE_DEBUG);

  driz_ret= drizzle_connect(con);
  if (driz_ret == DRIZZLE_RETURN_COULD_NOT_CONNECT)
  {
    char error[DRIZZLE_MAX_ERROR_SIZE];
    strncpy(error, drizzle_error(con), DRIZZLE_MAX_ERROR_SIZE);
    drizzle_quit(con);
    SKIP_IF_(driz_ret == DRIZZLE_RETURN_COULD_NOT_CONNECT, "%s(%s)", error,
             drizzle_strerror(driz_ret));
  }
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "drizzle_connect() : %s",
             drizzle_error(con));

  drizzle_query(con, "SELECT 1", 0, &driz_ret);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "SELECT 1 (%s)", drizzle_error(con));

  // Now that we know everything is good... lets push it.
  drizzle_close(con);

  int limit = 20;
  while (--limit)
  {
    driz_ret = drizzle_connect(con);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s(%s)", drizzle_error(con),
               drizzle_strerror(driz_ret));

    drizzle_query(con, "SELECT 1", 0, &driz_ret);
    ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "SELECT 1 (%s)", drizzle_error(con));

    // Now that we know everything is good... lets push it.
    drizzle_close(con);
  }

  driz_ret = drizzle_quit(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s", drizzle_strerror(driz_ret));

  return EXIT_SUCCESS;
}
