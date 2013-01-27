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

/**
 * @file
 * @brief Query definitions
 */

#include "config.h"

#include <libdrizzle/common.h>

drizzle_result_st *drizzle_query(drizzle_st *con,
                                 const char *query, size_t size,
                                 drizzle_return_t *ret_ptr)
{
  if (size == 0)
  {
    size= strlen(query);
  }
  return drizzle_command_write(con, NULL, DRIZZLE_COMMAND_QUERY,
                                   (unsigned char *)query, size, size, ret_ptr);
}

ssize_t drizzle_escape_string(drizzle_st *con, char **destination, const char *from, const size_t from_size)
{
  (void)con;
  const char *end;

  if (from == NULL || from_size == 0)
  {
    return -1;
  }

  size_t max_to_size= from_size * 2;
  *destination= (char*) malloc(max_to_size);

  if (destination == NULL)
  {
    return -1;
  }

  char *to= *destination;


  ssize_t to_size= 0;
  char newchar;

  for (end= from +from_size; from < end; from++)
  {
    newchar= 0;
    /* All multi-byte UTF8 characters have the high bit set for all bytes. */
    if (!(*from & 0x80))
    {
      switch (*from)
      {
      case 0:
        newchar= '0';
        break;
      case '\n':
        newchar= 'n';
        break;
      case '\r':
        newchar= 'r';
        break;
      case '\032':
        newchar= 'Z';
        break;
      case '\\':
        newchar= '\\';
        break;
      case '\'':
        newchar= '\'';
        break;
      case '"':
        newchar= '"';
        break;
      default:
        break;
      }
    }
    if (newchar != '\0')
    {
      if ((size_t)to_size + 2 > max_to_size)
      {
        free(to);
        *destination= NULL;
        return -1;
      }

      *to++= '\\';
      *to++= newchar;
      to_size++;
    }
    else
    {
      if ((size_t)to_size + 1 > max_to_size)
      {
        free(to);
        *destination= NULL;
        return -1;
      }

      *to++= *from;
    }
    to_size++;
  }

  *to= 0;

  return to_size;
}

bool drizzle_hex_string(char *to, const unsigned char *from, const size_t from_size)
{
  if (to == NULL || from == NULL || from_size == 0)
  {
    return false;
  }

  static const char hex_map[]= "0123456789ABCDEF";
  const unsigned char *from_end;

  for (from_end= from + from_size; from != from_end; from++)
  {
    *to++= hex_map[(*from) >> 4];
    *to++= hex_map[(*from) & 0xF];
  }

  *to= 0;

  return true;
}

bool drizzle_mysql_password_hash(char *to, const char *from, const size_t from_size)
{
  if (to == NULL || from == NULL || from_size == 0)
  {
    return false;
  }
  
  SHA1_CTX ctx;
  unsigned char hash_tmp1[SHA1_DIGEST_LENGTH];
  unsigned char hash_tmp2[SHA1_DIGEST_LENGTH];

  SHA1Init(&ctx);
  SHA1Update(&ctx, (const unsigned char*)from, from_size);
  SHA1Final(hash_tmp1, &ctx);

  SHA1Init(&ctx);
  SHA1Update(&ctx, hash_tmp1, SHA1_DIGEST_LENGTH);
  SHA1Final(hash_tmp2, &ctx);

  return drizzle_hex_string(to, hash_tmp2, SHA1_DIGEST_LENGTH);
}
