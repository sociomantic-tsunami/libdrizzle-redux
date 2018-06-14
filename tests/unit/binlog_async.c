/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (c) 2016 dunnhumby Germany GmbH
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
 * @brief callback for binlog errors
 * @details
 *
 * @param ret drizzle return value
 * @param connection drizzle connection object
 * @param context user defined context
 */
void binlog_error(drizzle_return_t VARIABLE_IS_NOT_USED ret,
  drizzle_st VARIABLE_IS_NOT_USED *connection, void VARIABLE_IS_NOT_USED *context);
void binlog_error(drizzle_return_t VARIABLE_IS_NOT_USED ret,
  drizzle_st VARIABLE_IS_NOT_USED *connection, void VARIABLE_IS_NOT_USED *context)
{
}

/**
 * @brief callback for binlog events
 *
 * @param event binlog event object
 * @param context user defined context
 */
void binlog_event(drizzle_binlog_event_st VARIABLE_IS_NOT_USED *event,
  void VARIABLE_IS_NOT_USED *context);
void binlog_event(drizzle_binlog_event_st VARIABLE_IS_NOT_USED *event,
  void VARIABLE_IS_NOT_USED *context)
{
}

int main(int argc, char *argv[]) {
  (void)argc;
  (void)argv;
  drizzle_binlog_st *binlog;
  drizzle_return_t driz_ret;

  opts = drizzle_options_create();
  drizzle_options_set_non_blocking(opts, true);

  con = drizzle_create(getenv("MYSQL_SERVER"),
                                   getenv("MYSQL_PORT") ? atoi(getenv("MYSQL_PORT"))
                                                        : DRIZZLE_DEFAULT_TCP_PORT,
                                   getenv("MYSQL_USER"),
                                   getenv("MYSQL_PASSWORD"),
                                   getenv("MYSQL_SCHEMA"), opts);
  ASSERT_NOT_NULL_(con, "Drizzle connection object creation error");

  binlog = drizzle_binlog_init(con, binlog_event, binlog_error,
    NULL, true);

  driz_ret = drizzle_binlog_start(binlog, 0, NULL, 0);

  ASSERT_EQ_(DRIZZLE_RETURN_IO_WAIT, driz_ret,
    "drizzle non blocking query: %s(%s)",
    drizzle_error(con), drizzle_strerror(driz_ret));
}
