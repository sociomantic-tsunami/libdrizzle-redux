/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
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
 * @brief Local Connection Declarations
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_local Local Connection Declarations
 * @ingroup drizzle_con
 * @{
 */

/**
 * Clear address info, freeing structs if needed.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 */
void drizzle_reset_addrinfo(drizzle_st *con);

/**
 * Send raw command to server, possibly in parts.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 * @param[in] result Caller allocated structure, or NULL to allocate one.
 * @param[in] command Command to run on server.
 * @param[in] data Data to send along with the command.
 * @param[in] size Size of the current chunk of data being sent.
 * @param[in] total Total size of all data being sent for command.
 * @param[out] ret_ptr Standard drizzle return value.
 * @return On success, a pointer to the (possibly allocated) structure. On
 *  failure this will be NULL.
 */
drizzle_result_st *drizzle_command_write(drizzle_st *con,
                                             drizzle_result_st *result,
                                             drizzle_command_t command,
                                             const void *data, size_t size,
                                             size_t total,
                                             drizzle_return_t *ret_ptr);
/**
 * Set TCP host and port for a connection.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 * @param[in] host Host to use for this connection, NULL for default value.
 * @param[in] port Port to use for this connection, 0 for default value.
 */
void drizzle_set_tcp(drizzle_st *con, const char *host, in_port_t port);

/**
 * Get unix domain socket for a connection.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 * @return Unix domain socket set for this connection, NULL if not set.
 */
const char *drizzle_uds(const drizzle_st *con);

/**
 * Set username and password for a connection.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 * @param[in] user Username to use for this connection.
 * @param[in] password Password to use for this connection.
 */
void drizzle_set_auth(drizzle_st *con, const char *user, const char *password);

/**
 * Set database for a connection.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 * @param[in] db Database to use with this connection.
 */
void drizzle_set_db(drizzle_st *con, const char *db);

/**
 * Set unix domain socket for a connection.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 * @param[in] uds Unix domain socket to use for this connection, NULL for
 *  defailt value.
 */
void drizzle_set_uds(drizzle_st *con, const char *uds);

/** @} */

#ifdef __cplusplus
}
#endif
