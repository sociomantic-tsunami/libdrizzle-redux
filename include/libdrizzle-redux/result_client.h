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
 * @brief Result Declarations for Clients
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_result_client Result Declarations for Clients
 * @ingroup drizzle_client_interface
 *
 * These functions read or buffer the result for a client command.
 * @{
 */

/**
 * Reads the next result in a multi-result return
 *
 * @param[in] con a drizzle object
 * @param[in,out] ret_ptr A pointer to a drizzle_return_t struct
 * to store the return status into
 * @return The result struct for the new object
 */
DRIZZLE_API
drizzle_result_st *drizzle_result_read(drizzle_st *con,
                                       drizzle_return_t *ret_ptr);

/**
 * Buffers a result set
 *
 * @param[in,out] result A result object
 * @return DRIZZLE_RETURN_OK upon success else DRIZZLE_RETURN_MEMORY
 */
DRIZZLE_API
drizzle_return_t drizzle_result_buffer(drizzle_result_st *result);

/**
 * Get result row packet size in bytes.
 *
 * @param[in] result Caller allocated structure.
 * @return size in bytes else 0
 */
DRIZZLE_API
size_t drizzle_result_row_size(drizzle_result_st *result);

/** @} */

#ifdef __cplusplus
}
#endif
