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
 * @param[in,out] result pointer to an unused structure that will be used for
 *                       the results, or NULL to allocate a new structure.
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

/*
 * Escape a string or encode a string in hexadecimal. The return value is the
 * size of the output string in to.
 */

DRIZZLE_API
ssize_t drizzle_escape_string(drizzle_st *con, char **to, const char *from, const size_t from_size);

/** @} */

#ifdef __cplusplus
}
#endif
