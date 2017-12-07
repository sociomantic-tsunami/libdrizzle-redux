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
 * @brief Connection Declarations for Clients
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_client Connection Declarations for Clients
 * @ingroup drizzle_client_interface
 * @{
 */

/**
 * Connect to server.
 *
 * @param[in] con Connection structure previously initialized with drizzle_create().
 * @return Standard drizzle return value.
 */
DRIZZLE_API
drizzle_return_t drizzle_connect(drizzle_st *con);

/**
 * Send quit command to server for a connection.
 *
 * @param[in] con Connection structure previously initialized with drizzle_create().
 * @return ret_ptr Standard drizzle return value.
 */
DRIZZLE_API
drizzle_return_t drizzle_quit(drizzle_st *con);

/**
 * Select a new default database for a connection.
 *
 * @param[in] con Connection structure previously initialized with drizzle_create().
 * @param[in] db Default database to select.
 * @return Standard drizzle return value
 */
DRIZZLE_API
drizzle_return_t drizzle_select_db(drizzle_st *con, const char *db);

/**
 * Send a shutdown message to the server.
 *
 * @param[in] con Connection structure previously initialized with drizzle_create().
 * @param[out] ret_ptr Standard drizzle return value.
 * @return On success, a pointer to the (possibly allocated) structure. On
 *  failure this will be NULL.
 */
DRIZZLE_API
drizzle_result_st *drizzle_shutdown(drizzle_st *con, drizzle_return_t *ret_ptr);

/**
 * Sends a query kill command to the server
 *
 * @param[out] con – A connection object
 * @param[in] connection_id – The connection ID to kill a query from
 * @param[out] ret_ptr A pointer to a drizzle_return_t to store the return status into
 * @return A newly allocated result object
*/
DRIZZLE_API
drizzle_result_st *drizzle_kill(drizzle_st *con,
                                uint32_t connection_id,
                                drizzle_return_t *ret_ptr);

/**
 * Send a ping request to the server.
 *
 * @param[in] con Connection structure previously initialized with drizzle_create().
 * @param[out] ret_ptr Standard drizzle return value.
 * @return On success, a pointer to the (possibly allocated) structure. On
 *  failure this will be NULL.
 */
DRIZZLE_API
drizzle_result_st *drizzle_ping(drizzle_st *con, drizzle_return_t *ret_ptr);

/** @} */

#ifdef __cplusplus
}
#endif
