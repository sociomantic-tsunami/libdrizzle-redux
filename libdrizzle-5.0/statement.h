/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
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

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

DRIZZLE_API
drizzle_stmt_st *drizzle_stmt_prepare(drizzle_st *con, const char *statement, size_t size, drizzle_return_t *ret_ptr);

DRIZZLE_API
drizzle_return_t drizzle_stmt_bind_param(drizzle_stmt_st *stmt, uint16_t param_num, drizzle_column_type_t type, void *data, uint32_t length, drizzle_bind_options_t options);

DRIZZLE_API
drizzle_return_t drizzle_stmt_execute(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_send_long_data(drizzle_stmt_st *stmt, uint16_t param_num, uint8_t *data, size_t len);

DRIZZLE_API
drizzle_return_t drizzle_stmt_reset(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_fetch(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_buffer(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_close(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_column_type_t drizzle_stmt_item_type(drizzle_stmt_st *stmt, uint16_t column_number);

DRIZZLE_API
void *drizzle_stmt_item_data(drizzle_stmt_st *stmt, uint16_t column_number);

DRIZZLE_API
uint32_t drizzle_stmt_item_length(drizzle_stmt_st *stmt, uint16_t column_number);

DRIZZLE_API
drizzle_bind_options_t drizzle_stmt_item_options(drizzle_stmt_st *stmt, uint16_t column_number);

DRIZZLE_API
uint16_t drizzle_stmt_column_count(drizzle_stmt_st *stmt);

DRIZZLE_API
uint64_t drizzle_stmt_affected_rows(drizzle_stmt_st *stmt);

DRIZZLE_API
uint64_t drizzle_stmt_insert_id(drizzle_stmt_st *stmt);

DRIZZLE_API
uint16_t drizzle_stmt_param_count(drizzle_stmt_st *stmt);

DRIZZLE_API
uint64_t drizzle_stmt_row_count(drizzle_stmt_st *stmt);



#ifdef __cplusplus
}
#endif

