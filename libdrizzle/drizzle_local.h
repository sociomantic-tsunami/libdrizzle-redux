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
 * @brief Local Drizzle Declarations
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_local Local Drizzle Declarations
 * @ingroup drizzle
 * @{
 */

/**
 * Set the error string.
 *
 * @param[in] drizzle Drizzle con structure
 * @param[in] function Name of function the error happened in.
 * @param[in] format Format and variable argument list of message.
 */
void drizzle_set_error(drizzle_st *con, const char *function, const char *format, ...);

/**
 * Free a connection structure.
 *
 * @param[in] con Connection structure previously initialized with
 *  drizzle_create(), drizzle_clone(), or related functions.
 */
void drizzle_free(drizzle_st *con);

/* Create a copy of drizzle_st instance
 *
 * @params[out] drizzle the target instance
 * @params[in] from the instance to clone
 * @return a newly allocated and configured copy of a drizzle connection
 */
drizzle_st *drizzle_clone(drizzle_st *drizzle, const drizzle_st *from);

/*
 * Initialize the shared ssl object, by calling pthread_once
 *
 * @params[in] initialized drizzle connection object
 * @return true if the call to pthread_once was successful, false otherwise
 */
bool drizzle_library_init(drizzle_st*);

/**
 * Initialize a result struct and add it to the connection object's result list
 *
 * @param[in] con initialized drizzle connection object
 * @return a newly allocated result object
 */
drizzle_result_st *drizzle_result_create(drizzle_st *con);

/*
 * Convert a char array to hex
 *
 * @param[out] to the destination string
 * @param[in] from the source string
 * @param[in] from_size the length of the source string
 *
 * @return true if the conversion succeeded, false if 'to' or 'from' are NULL or
 * from_size = 0
 */
bool drizzle_hex_string(char *to, const unsigned char *from, const size_t from_size);

/*
 * Create the hash of a password
 *
 * @param[out] to the destination string
 * @param[in] from the source string
 * @param[in] from_size the length of the source string
 *
 * @return true if the hash was generated successfully, false if 'to' or 'from'
 * are NULL or 'from_size' = 0
 */
bool drizzle_mysql_password_hash(char *to, const char *from, const size_t from_size);

/**
 * Log a message.
 *
 * @param[in] drizzle Drizzle con structure
 * @param[in] verbose Logging level of the message.
 * @param[in] format Format and variable argument list of message.
 * @param[in] args Variable argument list that has been initialized.
 */
void drizzle_log(drizzle_st *con, drizzle_verbose_t verbose, const char *format, va_list args);

/**
 * Log a fatal message, see drizzle_log() for argument details.
 */
static inline void drizzle_log_fatal(drizzle_st *con, const char *format, ...)
{
  va_list args;

  if (con->verbose >= DRIZZLE_VERBOSE_CRITICAL)
  {
    va_start(args, format);
    drizzle_log(con, DRIZZLE_VERBOSE_CRITICAL, format, args);
    va_end(args);
  }
}

/**
 * Log an error message, see drizzle_log() for argument details.
 */
static inline void drizzle_log_error(drizzle_st *con, const char *format, ...)
{
  va_list args;

  if (con->verbose >= DRIZZLE_VERBOSE_ERROR)
  {
    va_start(args, format);
    drizzle_log(con, DRIZZLE_VERBOSE_ERROR, format, args);
    va_end(args);
  }
}

/**
 * Log an info message, see drizzle_log() for argument details.
 */
static inline void drizzle_log_info(drizzle_st *con, const char *format, ...)
{
  va_list args;

  if (con->verbose >= DRIZZLE_VERBOSE_INFO)
  {
    va_start(args, format);
    drizzle_log(con, DRIZZLE_VERBOSE_INFO, format, args);
    va_end(args);
  }
}

/**
 * Log a debug message, see drizzle_log() for argument details.
 */
static inline void drizzle_log_debug(drizzle_st *con, const char *format, ...)
{
  va_list args;

  if (con->verbose >= DRIZZLE_VERBOSE_DEBUG)
  {
    va_start(args, format);
    drizzle_log(con, DRIZZLE_VERBOSE_DEBUG, format, args);
    va_end(args);
  }
}

/** @} */

#ifdef __cplusplus
}
#endif
