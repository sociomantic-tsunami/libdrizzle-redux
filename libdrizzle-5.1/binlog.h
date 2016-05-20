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

/**
* Initializes a binlog object for the connection and sets the event callback
* functions
*
* @param[in] con              The connection the binlog retrieval will be on
* @param[in] binlog_fn        The function callback defined in (drizzle_binlog_fn)()
* @param[in] error_fn         The function callback defined in (drizzle_binlog_error_fn)()
* @param[in] context          A pointer to user data which will be used for the
*                             callback functions
* @param[in] verify_checksums Set to true if MySQL 5.6 and higher checksums
*                             should be verified
* @return    On success, a pointer to the (possibly allocated) structure. On
*            failure this will be NULL.
*/
DRIZZLE_API
drizzle_binlog_st *drizzle_binlog_init(drizzle_st *con,
                                       drizzle_binlog_fn *binlog_fn,
                                       drizzle_binlog_error_fn *error_fn,
                                       void *context,
                                       bool verify_checksums);

/**
* Frees a binlog object created with drizzle_binlog_init()
*
* @param[in] binlog The binlog object to be freed
*/
DRIZZLE_API
void drizzle_binlog_free(drizzle_binlog_st *binlog);

/**
* Start the binlog transaction. Set the server_id to 0 to disconnect
* automatically at the end of the last log.
*
* @param[in] binlog         A binlog object created using drizzle_binlog_init()
* @param[in] server_id      A unique server ID (or 0) to connect to the
*                           MySQL server with
* @param[in] file           The start binlog file, can be empty to start at the
*                           first known file
* @param[in] start_position The position of the binlog file to start at, a value
*                           of less than 4 is set to 4 due to the binlog header
*                           taking the first 4 bytes
* @return A Drizzle return type. DRIZZLE_RETURN_OK upon success.
*/
DRIZZLE_API
drizzle_return_t drizzle_binlog_start(drizzle_binlog_st *binlog,
                                        uint32_t server_id,
                                        const char *file,
                                        uint32_t start_position);

/**
* Get the timestamp for the event received by the event callback
*
* @param[in] event The event from the binlog stream
* @return The timestamp for the binlog event
*/
DRIZZLE_API
uint32_t drizzle_binlog_event_timestamp(drizzle_binlog_event_st *event);

/**
* Get the event type for the event received by the event callback
*
* @param[in] event The event from the binlog stream
* @return The event type of the binlog event
*/
DRIZZLE_API
drizzle_binlog_event_types_t drizzle_binlog_event_type(drizzle_binlog_event_st *event);

/**
* Get the server_id for the event received by the event callback
*
* @param[in] event The event from the binlog stream
* @return The server_id for the binlog event
*/
DRIZZLE_API
uint32_t drizzle_binlog_event_server_id(drizzle_binlog_event_st *event);

/**
* Get the length of the event data (excluding header) received by the event
* callback
*
* @param[in] event The event from binlog stream
* @return The event data length
*/
DRIZZLE_API
uint32_t drizzle_binlog_event_length(drizzle_binlog_event_st *event);

/**
* Get the next event position from the event received by the event callback
*
* @param[in] event The event from the binlog stream
* @return The next event position
*/
DRIZZLE_API
uint32_t drizzle_binlog_event_next_pos(drizzle_binlog_event_st *event);

/**
* Get the flags for the event received by the event callback
*
* @param[in] event The event from the binlog stream
* @return The event flags
*/
DRIZZLE_API
uint16_t drizzle_binlog_event_flags(drizzle_binlog_event_st *event);

/**
* Get the flags for the event received by the event callback
*
* @param[in] event The event from the binlog stream
* @return The extra event flags
*/
DRIZZLE_API
uint16_t drizzle_binlog_event_extra_flags(drizzle_binlog_event_st *event);

/**
* Get the event data for the event received by the event callback
*
* @param[in] event The event from the binlog stream
* @return A pointer to the event data
*/
DRIZZLE_API
const unsigned char *drizzle_binlog_event_data(drizzle_binlog_event_st *event);

/**
* Get the raw event data (including header) for the event received by the event
* callback
*
* @param[in] event The event from the binlog stream
* @return A pointer to the raw event data
*/
DRIZZLE_API
const unsigned char *drizzle_binlog_event_raw_data(drizzle_binlog_event_st *event);

/**
* Get the length of the raw event data (including header) for the event
* received by the event callback
*
* @param[in] event The event from the binlog stream
* @return The length of the raw event data
*/
DRIZZLE_API
uint32_t drizzle_binlog_event_raw_length(drizzle_binlog_event_st *event);

#ifdef __cplusplus
}
#endif
