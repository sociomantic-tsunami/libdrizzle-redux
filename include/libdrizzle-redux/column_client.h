/*
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2008-2013 Drizzle Developer Group
 * Copyright (C) 2008 Eric Day (eday@oddments.org)
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

/**
 * @file
 * @brief Column Declarations for Clients
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_column_client Column Declarations for Clients
 * @ingroup drizzle_client_interface
 *
 * These functions are used to get detailed column information. This information
 * is usually sent as the first part of a result set. There are both buffered
 * and unbuffered functions provided.
 * @{
 */

/**
 * Skips the next column in a result set when using drizzle_column_read() to
 * get the column data
 *
 * @param[in,out] result pointer to the structure to read from.
 * @return A return status code, DRIZZLE_RETURN_OK upon success
 */
DRIZZLE_API
drizzle_return_t drizzle_column_skip(drizzle_result_st *result);

/**
 * Skips all columns in a result set when using drizzle_column_read() to
 * get the column data
 *
 * @param[in,out] result pointer to the structure to read from.
 * @return A return status code, DRIZZLE_RETURN_OK upon success
 */
DRIZZLE_API
drizzle_return_t drizzle_column_skip_all(drizzle_result_st *result);

/**
 * Read column information.
 *
 * @param[in,out] result  pointer to the structure to read from.
 * @param[out]    ret_ptr Standard libdrizzle return value.
 * @return column if there is valid data, NULL if there are no more columns.
 */
DRIZZLE_API
drizzle_column_st *drizzle_column_read(drizzle_result_st *result,
                                       drizzle_return_t *ret_ptr);

/**
 * Buffer all columns in result structure.
 *
 * @param[in,out] result pointer to the structure to read from.
 * @return Standard drizzle return value.
 */
DRIZZLE_API
drizzle_return_t drizzle_column_buffer(drizzle_result_st *result);

/**
 * Get next buffered column from a result structure.
 *
 * @param[in,out] result pointer to the structure to read from.
 * @return A column object
 */
DRIZZLE_API
drizzle_column_st *drizzle_column_next(drizzle_result_st *result);

/**
 * Get previous buffered column from a result structure.
 *
 * @param[in,out] result pointer to the structure to read from.
 * @return A column object
 */
DRIZZLE_API
drizzle_column_st *drizzle_column_prev(drizzle_result_st *result);

/**
 * Seeks to a given column in a buffered column result set
 *
 * @param[in,out] result pointer to the structure to read from.
 * @param[in]     column The column number
 */
DRIZZLE_API
void drizzle_column_seek(drizzle_result_st *result, uint16_t column);

/**
 * Gets a given column in a column buffered result set
 *
 * @param[in,out] result pointer to the structure to read from.
 * @param[in]     column The column number
 * @return A column object
 */
DRIZZLE_API
drizzle_column_st *drizzle_column_index(const drizzle_result_st *result,
                                        uint16_t column);

/**
 * Gets the column number in a buffered or unbuffered column result set
 *
 * @param[in,out] result pointer to the structure to read from.
 * @return        The column number
 */
DRIZZLE_API
uint16_t drizzle_column_current(const drizzle_result_st *result);

/** @} */

#ifdef __cplusplus
}
#endif
