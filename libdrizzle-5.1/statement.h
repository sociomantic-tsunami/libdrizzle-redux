/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012-2013 Drizzle Developer Group
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
drizzle_return_t drizzle_stmt_execute(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_send_long_data(drizzle_stmt_st *stmt, uint16_t param_num, unsigned char *data, size_t len);

DRIZZLE_API
drizzle_return_t drizzle_stmt_reset(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_fetch(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_buffer(drizzle_stmt_st *stmt);

DRIZZLE_API
drizzle_return_t drizzle_stmt_close(drizzle_stmt_st *stmt);

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

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_tiny(drizzle_stmt_st *stmt, uint16_t param_num, uint8_t value, bool is_unsigned);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_short(drizzle_stmt_st *stmt, uint16_t param_num, uint16_t value, bool is_unsigned);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_int(drizzle_stmt_st *stmt, uint16_t param_num, uint32_t value, bool is_unsigned);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_bigint(drizzle_stmt_st *stmt, uint16_t param_num, uint64_t value, bool is_unsigned);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_double(drizzle_stmt_st *stmt, uint16_t param_num, double value);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_float(drizzle_stmt_st *stmt, uint16_t param_num, float value);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_string(drizzle_stmt_st *stmt, uint16_t param_num, const char *value, size_t length);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_null(drizzle_stmt_st *stmt, uint16_t param_num);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_time(drizzle_stmt_st *stmt, uint16_t param_num, uint32_t days, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t microseconds, bool is_negative);

DRIZZLE_API
drizzle_return_t drizzle_stmt_set_timestamp(drizzle_stmt_st *stmt, uint16_t param_num, uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t microseconds);

DRIZZLE_API
bool drizzle_stmt_get_is_null_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr);

DRIZZLE_API
bool drizzle_stmt_get_is_null(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr);

DRIZZLE_API
bool drizzle_stmt_get_is_unsigned_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr);

DRIZZLE_API
bool drizzle_stmt_get_is_unsigned(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr);

DRIZZLE_API
const char *drizzle_stmt_get_string_from_name(drizzle_stmt_st *stmt, const char *column_name, size_t *len, drizzle_return_t *ret_ptr);

DRIZZLE_API
const char *drizzle_stmt_get_string(drizzle_stmt_st *stmt, uint16_t column_number, size_t *len, drizzle_return_t *ret_ptr);

DRIZZLE_API
uint32_t drizzle_stmt_get_int(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr);

DRIZZLE_API
uint32_t drizzle_stmt_get_int_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr);

DRIZZLE_API
uint64_t drizzle_stmt_get_bigint_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr);

DRIZZLE_API
uint64_t drizzle_stmt_get_bigint(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr);

DRIZZLE_API
double drizzle_stmt_get_double(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr);

DRIZZLE_API
double drizzle_stmt_get_double_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr);

#ifdef __cplusplus
}
#endif

