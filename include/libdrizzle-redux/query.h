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
 * @brief Query Declarations
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_query Query Declarations
 *
 * @ingroup drizzle_client_interface
 * These functions are used to issue queries on a connection. Single queries are
 * made using the drizzle_query function, or you can queue multiple queries and
 * run them concurrently using the other query functions.
 * @{
 */

/**
 * Send query to server. A \ref drizzle_result_st will be created for the
 * results.
 *
 * @param[in] con connection to use to send the query.
 * @param[in] query query string to send.
 * @param[in] size length of the query string in bytes.
 * @param[out] ret_ptr pointer to the result code.
 * @return result, a pointer to the newly allocated result structure, or NULL
 *         if the allocation failed.
 */
DRIZZLE_API
drizzle_result_st *drizzle_query(drizzle_st *con,
                                 const char *query, size_t size,
                                 drizzle_return_t *ret_ptr);

/**
 * Escape a string for an SQL query. The to parameter is allocated by the
 * function and needs to be freed by the application when finished with.
 *
 * This function does not escape '%' or '_', if you need to, call
 * drizzle_escape_str with `is_pattern` set to `true`.
 *
 * @param[in] con a connection object
 * @param[in,out] to the destination string
 * @param[in] from the source string
 * @param[in] from_size the length of the source string
 * @return the length of the ‘to’ string or -1 upon error due to empty
 *         parameters or overflow
 */
DRIZZLE_API
ssize_t drizzle_escape_string(const drizzle_st *con, char **to, const char *from, const size_t from_size);

/**
 * Escape a string for an SQL query, optionally for pattern matching.
 *
 * This function escapes the following characters:
 *
 * \verbatim
'\0' (0x00), '\'' (0x27), '"' (0x22), '\b' (0x08), '\n' (0x0A),
'\r' (0x0D), '\t' (0x09), '\Z' (0x26), '\\' (0x5C).
\endverbatim
 * In case \p is_pattern is set to `true`, '%' (0x25) and '_' (0x5F)
 * will be escaped as well.
 *
 * The to parameter is allocated by the function and needs to be freed by
 * the application when finished with.
 *
 * @param[in] con a connection object
 * @param[in,out] to the destination string
 * @param[in] from the source string
 * @param[in] from_size the length of the source string
 * @param[in] is_pattern whether to escape '%' and '_'.
 *                       If set to `true`, they will be escaped,
 *                       so the string can be used in a LIKE clause for example.
 * @return the length of the ‘to’ string or -1 upon error due to empty
 *         parameters or overflow
 */
DRIZZLE_API
ssize_t drizzle_escape_str(const drizzle_st *con, char **to, const char *from, const size_t from_size, bool is_pattern);

/** @} */

#ifdef __cplusplus
}
#endif
