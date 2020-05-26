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

#include "config.h"

#include "src/common.h"

#include <libdrizzle-redux/ssl.h>

#if defined(USE_OPENSSL)
# include <openssl/ssl.h>

drizzle_return_t drizzle_set_ssl(drizzle_st *con, const char *key, const char *cert, const char *ca, const char *capath, const char *cipher)
{
  con->ssl_context= SSL_CTX_new(TLS_client_method());

  const long required_ssl_options = (SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3);
  long ssl_options =
      SSL_CTX_set_options((SSL_CTX *)con->ssl_context, required_ssl_options);
  if (!(ssl_options & required_ssl_options))
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "Cannot set the SSL protocol options");
    return DRIZZLE_RETURN_SSL_ERROR;
  }

  if (cipher)
  {
    if (SSL_CTX_set_cipher_list((SSL_CTX*)con->ssl_context, cipher) != 1)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "Cannot set the SSL cipher list");
      return DRIZZLE_RETURN_SSL_ERROR;
    }
  }

  if (SSL_CTX_load_verify_locations((SSL_CTX*)con->ssl_context, ca, capath) != 1)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "Cannot load the SSL certificate authority file");
    return DRIZZLE_RETURN_SSL_ERROR;
  }

  if (cert)
  {
    if (SSL_CTX_use_certificate_file((SSL_CTX*)con->ssl_context, cert, SSL_FILETYPE_PEM) != 1)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "Cannot load the SSL certificate file");
      return DRIZZLE_RETURN_SSL_ERROR;
    }

    if (!key)
      key= cert;

    if (SSL_CTX_use_PrivateKey_file((SSL_CTX*)con->ssl_context, key, SSL_FILETYPE_PEM) != 1)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "Cannot load the SSL key file");
      return DRIZZLE_RETURN_SSL_ERROR;
    }

    if (SSL_CTX_check_private_key((SSL_CTX*)con->ssl_context) != 1)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__, "Error validating the SSL private key");
      return DRIZZLE_RETURN_SSL_ERROR;
    }
  }

  return DRIZZLE_RETURN_OK;
}

#else

drizzle_return_t drizzle_set_ssl(drizzle_st*, const char*, const char*, const char*, const char*, const char*)
{
  return DRIZZLE_RETURN_INVALID_ARGUMENT;
}

#endif
