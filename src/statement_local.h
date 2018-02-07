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

drizzle_return_t drizzle_stmt_set_param(drizzle_stmt_st *stmt, uint16_t param_num, drizzle_column_type_t type, const void *data, size_t length, bool is_unsigned);

char *long_to_string(drizzle_bind_st *param, uint32_t val);

char *longlong_to_string(drizzle_bind_st *param, uint64_t val);

char *double_to_string(drizzle_bind_st *param, double val);

char *time_to_string(drizzle_bind_st *param, drizzle_datetime_st *time);

char *timestamp_to_string(drizzle_bind_st *param, drizzle_datetime_st *timestamp);

uint16_t drizzle_stmt_column_lookup(drizzle_result_st *result, const char *column_name, drizzle_return_t *ret_ptr);

#ifdef __cplusplus
}

template <typename DST_TYPE, typename SRC_SIGNED, typename SRC_UNSIGNED>
DST_TYPE signedness_cast(const unsigned char *src, bool is_unsigned);

template<typename U>
U drizzle_get_integer(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr);
#endif
