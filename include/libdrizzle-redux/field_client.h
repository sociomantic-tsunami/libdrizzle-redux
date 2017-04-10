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
 * @brief Field Declarations for Clients
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_field_client Field Declarations for Clients
 * @ingroup drizzle_client_interface
 *
 * These functions allow you to access fields in a result set if the result is
 * unbuffered. If the result is buffered, you can access the fields through the
 * row functions.
 * @{
 */

/**
 * Read field for unbuffered result, possibly in parts. This is especially
 * useful for blob streaming, since the client does not need to buffer the
 * entire blob.
 *
 * The return value is a pointer into a buffer internal to \a result and should
 * not be freed. It is valid only until the next call to a drizzle function for
 * this connection.
 *
 * @param[in,out] result  The result handle for the response being read.
 * @param[out]    offset  The offset within the field of the data that was read.
 * @param[out]    size    The number of bytes returned.
 * @param[out]    total   The total size of the field being read.
 * @param[out]    ret_ptr DRIZZLE_RETURN_*
 * @return A pointer to \a size bytes of field data.
 */
DRIZZLE_API
drizzle_field_t drizzle_field_read(drizzle_result_st *result, uint64_t *offset,
                                   size_t *size, uint64_t *total,
                                   drizzle_return_t *ret_ptr);


/**
 * Read and buffer the entire field for an unbuffered row read.
 * The return value is a newly allocated buffer and must be freed using
 * drizzle_field_free when no longer needed.
 *
 * The buffering is done by keeping an array of char* pointers, each representing
 * a field from a result row. Memory is (re)allocated lazily as required by the
 * size of the field to buffer. Moreover the allocation is conservative and
 * allocates 'new-size + 1' bytes.
 *
 * For further details please refer to the inline documentation in
 * src/field.cc
 *
 * @param[in]  result  A result object
 * @param[in]  total   The total size of the field, to be written to by the function
 * @param[out] ret_ptr A pointer to a drizzle_return_t to store the return status into
 * @return The field data
 */
DRIZZLE_API
drizzle_field_t drizzle_field_buffer(drizzle_result_st *result, size_t *total,
                                     drizzle_return_t *ret_ptr);

/**
 * Frees field data for unbuffered row reads
 *
 * @param field The field data to free
 */
DRIZZLE_API
void drizzle_field_free(drizzle_field_t field);

/** @} */

#ifdef __cplusplus
}
#endif
