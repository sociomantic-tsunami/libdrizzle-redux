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

/**
 * @brief Macro to setter and getter API functions for a specific drizzle_option
 * @details [long description]
 *
 * @param api_func_set pointer to a setter function
 * @param api_func_get pointer to a getter function
 *
 * @return [description]
 */
#define CHECK_DRIZZLE_OPTION(api_func_set, api_func_get) \
do \
{ \
  api_func_set(opts, true); \
  ASSERT_TRUE(api_func_get(opts)); \
  api_func_set(NULL, false); \
  ASSERT_TRUE(api_func_get(opts)); \
  ASSERT_FALSE(api_func_get(NULL)); \
  api_func_set(opts, false); \
  ASSERT_FALSE(api_func_get(opts)); \
} \
while (0)


int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;

  opts = drizzle_options_create();
  CHECK_DRIZZLE_OPTION(drizzle_options_set_non_blocking, drizzle_options_get_non_blocking);
  CHECK_DRIZZLE_OPTION(drizzle_options_set_raw_scramble, drizzle_options_get_raw_scramble);
  CHECK_DRIZZLE_OPTION(drizzle_options_set_found_rows, drizzle_options_get_found_rows);
  CHECK_DRIZZLE_OPTION(drizzle_options_set_interactive, drizzle_options_get_interactive);
  CHECK_DRIZZLE_OPTION(drizzle_options_set_multi_statements, drizzle_options_get_multi_statements);
  CHECK_DRIZZLE_OPTION(drizzle_options_set_auth_plugin, drizzle_options_get_auth_plugin);

  drizzle_options_set_socket_owner(NULL, DRIZZLE_SOCKET_OWNER_CLIENT);
  ASSERT_EQ(DRIZZLE_SOCKET_OWNER_NATIVE, drizzle_options_get_socket_owner(opts));
  drizzle_options_set_socket_owner(opts, DRIZZLE_SOCKET_OWNER_CLIENT);
  ASSERT_EQ(DRIZZLE_SOCKET_OWNER_NATIVE, drizzle_options_get_socket_owner(NULL));

  con = drizzle_create(getenv("MYSQL_SERVER"),
                                   getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                                        : DRIZZLE_DEFAULT_TCP_PORT,
                                   getenv("MYSQL_USER"),
                                   getenv("MYSQL_PASSWORD"),
                                   getenv("MYSQL_SCHEMA"), opts);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  close_connection_on_exit();
}
