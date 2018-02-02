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

/**
 * @file
 * @brief Result Declarations
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_result Result Declarations
 * @ingroup drizzle_client_interface
 * @ingroup drizzle_server_interface
 *
 * These are core result functions used by both clients and servers.
 * @{
 */

/**
 * Frees all result objects for a given connection object
 *
 * @param[in,out] result a connection object
 */
DRIZZLE_API
void drizzle_result_free(drizzle_result_st *result);

/**
 * Frees all result objects for a given connection object
 *
 * @param[in,out] con a connection object
 */
DRIZZLE_API
void drizzle_result_free_all(drizzle_st *con);

/**
 * Gets the drizzle object from a given result object
 *
 * @param[in] result A result object
 * @return The drizzle object associated to the result object
 */
DRIZZLE_API
drizzle_st *drizzle_result_drizzle_con(drizzle_result_st *result);

/**
 * Tests to see if an EOF packet has been hit
 *
 * @param[in] result A result object
 * @return true on EOF else false
 */
DRIZZLE_API
bool drizzle_result_eof(drizzle_result_st *result);

/**
 * Get error or information message from result set
 *
 * @param[in] result a result object
 * @return The message to be returned
 */
DRIZZLE_API
const char *drizzle_result_message(drizzle_result_st *result);

/**
 * Gets the error code from a result set
 *
 * @param[in] result a result object
 * @return The error code
 */
DRIZZLE_API
uint16_t drizzle_result_error_code(drizzle_result_st *result);

/**
 * Gets the SQL state from a result set
 *
 * @param[in] result A result object
 * @return The SQL state string
 */
DRIZZLE_API
const char *drizzle_result_sqlstate(drizzle_result_st *result);

/**
  * Gets the warning count from a result set
 *
 * @param[in] result A result object
 * @return The warning count
 */
DRIZZLE_API
uint16_t drizzle_result_warning_count(drizzle_result_st *result);

/**
 * Gets the insert ID for an auto_increment column in a result set
 *
 * @param[in] result A result object
 * @return The inserted ID
 */
DRIZZLE_API
uint64_t drizzle_result_insert_id(drizzle_result_st *result);

/**
 * Gets the affected row count from a result set
 *
 * @param[in] result A result object
 * @return The affected row count
 */
DRIZZLE_API
uint64_t drizzle_result_affected_rows(drizzle_result_st *result);

/**
 * Gets the column count from a result set
 *
 * @param[in] result A result object
 * @return The column count
 */
DRIZZLE_API
uint16_t drizzle_result_column_count(drizzle_result_st *result);

/**
 * Gets the row count from a result set buffered with drizzle_result_buffer()
 *
 * @param[in] result A result object
 * @return The row count
 */
DRIZZLE_API
uint64_t drizzle_result_row_count(drizzle_result_st *result);

/** @} */

#ifdef __cplusplus
}
#endif
