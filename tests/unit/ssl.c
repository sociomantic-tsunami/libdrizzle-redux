/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright: Copyright (c) 2018 sociomantic labs GmbH. All rights reserved
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

/**
 *  Test encrypted connections to a MySQL server
 *
 *  The key, certificate and certificate authority must have the default MySQL
 *  SSL filenames
 *   - client-key.pem
 *   - client-cert.pem
 *   - ca.pem
 *
 *  The path to the certificate and key files must be specified with the
 *  environment variable DRIZZLE_MYSQL_CA_PATH
 *
 *  Note that establishing the encrypted connection to the MySQL server is not
 *  tested. This is due to a bug where openssl doesn't work with an underlying
 *  non-blocking socket.
 */
int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  drizzle_return_t driz_ret;
  char client_key[256];
  char client_cert[256];
  char server_ca[256];
  const char *ca_path;
  const char *cipher = "AES256-SHA";
  drizzle_st *con;

  con = drizzle_create(getenv("MYSQL_SOCK"),
                        getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                            : DRIZZLE_DEFAULT_TCP_PORT,
                        getenv("MYSQL_USER"), getenv("MYSQL_PASSWORD"),
                        getenv("MYSQL_SCHEMA"), 0);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  driz_ret = drizzle_connect(con);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "%s(%s)", drizzle_error(con),
             drizzle_strerror(driz_ret));

  // Check if server has SSL capabilities enabled.
  ASSERT_NEQ_((drizzle_capabilities(con) & DRIZZLE_CAPABILITIES_SSL), 0,
    "SSL not supported by the MySQL server");

  // Check environment variable DRIZZLE_MYSQL_CA_PATH is set
  ca_path = getenv("DRIZZLE_MYSQL_CA_PATH");
  ASSERT_NOT_NULL_(ca_path,
    "DRIZZLE_MYSQL_CA_PATH (path to certificate and key files) not set");

  sprintf(client_key, "%s/%s", ca_path ,"client-key.pem");
  sprintf(client_cert, "%s/%s", ca_path ,"client-cert.pem");
  sprintf(server_ca, "%s/%s", ca_path,"ca.pem");

  driz_ret = drizzle_set_ssl(con, "invalid_ssl_key", client_cert, server_ca,
    ca_path, NULL);
  ASSERT_EQ_(DRIZZLE_RETURN_SSL_ERROR, driz_ret, "drizzle_set_ssl(): %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));

  driz_ret = drizzle_set_ssl(con, client_key, "invalid_ssl_cert", server_ca,
    ca_path, NULL);
  ASSERT_EQ_(DRIZZLE_RETURN_SSL_ERROR, driz_ret, "drizzle_set_ssl(): %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));

  driz_ret = drizzle_set_ssl(con, client_key, client_cert, "invalid_ssl_ca",
    ca_path, NULL);
  ASSERT_EQ_(DRIZZLE_RETURN_SSL_ERROR, driz_ret, "drizzle_set_ssl(): %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));

  driz_ret = drizzle_set_ssl(con, client_key, client_cert, server_ca, ca_path,
    cipher);
  ASSERT_EQ_(DRIZZLE_RETURN_OK, driz_ret, "drizzle_set_ssl(): %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));

  return EXIT_SUCCESS;
}
