/*
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
 *
 * Sets the SSL data
 *
 * param[out] con A connection object
 * param[in] key The path to a key file
 * param[in] cert The path to a certificate file
 * param[in] ca The path to a certificate authority file
 * param[in] capath The path to a directory that contains trusted CA
           certificate files
 * param[in] cipher A list of allowed ciphers for SSL encryption
 * return A return status code, DRIZZLE_RETURN_OK upon success
 *
 */
DRIZZLE_API
drizzle_return_t drizzle_set_ssl(drizzle_st *con, const char *key,
    const char *cert, const char *ca, const char *capath, const char *cipher);

/**
 * @brief Get a pointer to the name of the cipher used current established
 * connection.
 * If the cipher is NULL, cipher is set to "(NONE)".
 *
 * param[in] con A connection object
 * param[out] cipher reference to a const char pointer
 * @return A return status code, DRIZZLE_RETURN_OK upon success
 */
DRIZZLE_API
drizzle_return_t drizzle_ssl_get_cipher_name(drizzle_st * con,
    const char **cipher_name);

#ifdef __cplusplus
}
#endif

/**
 * @brief Get the string which indicates the SSL/TLS protocol version that first
 * defined the cipher, or in the case the SSL/TLS protocol negotiated between
 * client and server.
 *
 * This is currently SSLv2 or TLSv1/SSLv3. In some cases it
 * should possibly return "TLSv1.2" but does not;
 * If cipher is NULL, "(NONE)" `cipher` is set to "(NONE)".
 *
 * param[out] con A connection object
 * @param cipher_version reference to a const char pointer
 * @return A return status code DRIZZLE_RETURN_OK upon success
 */
DRIZZLE_API
drizzle_return_t drizzle_ssl_cipher_get_version(drizzle_st *con,
    const char **cipher_version);
