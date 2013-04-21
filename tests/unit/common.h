/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
 *
 * Copyright (C) 2013 Drizzle Developer Group
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

#include <stdlib.h>
#include <stdio.h>

#include <libdrizzle-5.1/libdrizzle.h>

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern drizzle_st *con;

/* Common connection setup used by the unit tests. 
 */
extern void set_up_connection(void);

extern void close_connection_on_exit(void);

/* For unit tests that make tables, etc.: delete-and-recreate the schema on setup, and delete the schema on teardown. */
extern void set_up_schema(void);
extern void tear_down_schema(void);

/* Perform a query; assign the result to 'result' and the returncode to 'driz_ret'; assert that the returncode is DRIZZLE_RETURN_OK. */
#define CHECKED_QUERY(cmd) result = drizzle_query(con, cmd, 0, &driz_ret); \
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, from \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), cmd);

/* Call a libdrizzle function that just returns a drizzle_return_t. Assign the return to 'driz_ret' and assert that it is DRIZZLE_RETURN_OK. */
#define CHECK(s) driz_ret = (s); \
    ASSERT_EQ_(driz_ret, DRIZZLE_RETURN_OK, "Error (%s): %s, in \"%s\"", drizzle_strerror(driz_ret), drizzle_error(con), #s);

#ifdef __cplusplus
} /* extern "C" */
#endif
