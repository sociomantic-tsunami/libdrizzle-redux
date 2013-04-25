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
 * @brief Packing definitions
 */

#include "config.h"
#include "libdrizzle/common.h"

/*
 * Private declarations
 */

/**
 * @addtogroup drizzle_pack_private Private Packing Functions
 * @ingroup drizzle_pack
 * @{
 */

/**
 * Compute hash from password and scramble.
 */
static drizzle_return_t _pack_scramble_hash(drizzle_st *con,
                                            unsigned char *buffer);

/** @} */

/*
 * Public definitions
 */

unsigned char *drizzle_pack_length(uint64_t number, unsigned char *ptr)
{
  if (number < 251)
  {
    ptr[0]= (uint8_t)number;
    ptr++;
  }
  else if (number < 65536)
  {
    ptr[0]= 252;
    ptr++;
    drizzle_set_byte2(ptr, number);
    ptr+= 2;
  }
  else if (number < 16777216)
  {
    ptr[0]= 253;
    ptr++;
    drizzle_set_byte3(ptr, number);
    ptr+= 3;
  }
  else
  {
    ptr[0]= 254;
    ptr++;
    drizzle_set_byte8(ptr, number);
    ptr+= 8;
  }

  return ptr;
}

uint64_t drizzle_unpack_length(drizzle_st *con, drizzle_return_t *ret_ptr)
{
  uint64_t length;
  uint8_t bytes;

  drizzle_return_t unused_ret;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused_ret;
  }

  if (con == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return 0;
  }

  if (con->buffer_ptr[0] < 251)
  {
    length= (uint64_t)(con->buffer_ptr[0]);
    bytes= 1;
  }
  else if (con->buffer_ptr[0] == 251)
  {
    con->buffer_ptr++;
    con->buffer_size--;
    con->packet_size--;

    *ret_ptr= DRIZZLE_RETURN_NULL_SIZE;
    return 0;
  }
  else if (con->buffer_ptr[0] == 252 && con->buffer_size > 2)
  {
    length= drizzle_get_byte2(con->buffer_ptr + 1);
    bytes= 3;
  }
  else if (con->buffer_ptr[0] == 253 && con->buffer_size > 3)
  {
    length= drizzle_get_byte3(con->buffer_ptr + 1);
    bytes= 4;
  }
  else if (con->buffer_size > 8)
  {
    length= drizzle_get_byte8(con->buffer_ptr + 1);
    bytes= 9;
  }
  else
  {
    *ret_ptr= DRIZZLE_RETURN_IO_WAIT;
    return 0;
  }

  con->buffer_ptr+= bytes;
  con->buffer_size-= bytes;
  con->packet_size-= bytes;

  *ret_ptr= DRIZZLE_RETURN_OK;
  return length;
}

unsigned char *drizzle_pack_string(char *string, unsigned char *ptr)
{
  if (string == NULL)
  {
    return NULL;
  }

  size_t size= strlen(string);

  ptr= drizzle_pack_length(size, ptr);
  if (size > 0)
  {
    memcpy(ptr, string, size);
    ptr+= size;
  }

  return ptr;
}

unsigned char *drizzle_pack_binary(unsigned char *data, size_t len, unsigned char *ptr)
{
  ptr= drizzle_pack_length(len, ptr);
  if (len > 0)
  {
    memcpy(ptr, data, len);
    ptr+= len;
  }

  return ptr;
}

unsigned char *drizzle_pack_time(drizzle_datetime_st *time, unsigned char *ptr)
{
  uint8_t length= 0;

  /* NOTE: MySQL has a bug here and doesn't follow this part of the protocol
   * when packing, we will for now, no idea if it works
   * */
  if (time->microsecond)
  {
    drizzle_set_byte4(ptr+9, time->microsecond);
    length= 12;
  }

  if (length || time->day || time->hour || time->minute || time->second)
  {
    ptr[1]= (time->negative) ? 1 : 0;
    drizzle_set_byte4(ptr+2, time->day);
    drizzle_set_byte1(ptr+6, time->hour);
    drizzle_set_byte1(ptr+7, time->minute);
    drizzle_set_byte1(ptr+8, time->second);
    /* If no microseconds, then we are packing 8 bytes */
    if (!length)
      length= 8;
  }

  /* If nothing is set then we are sending a 0 length time */

  drizzle_set_byte1(ptr, length);
  return ptr + 1 + length;
}

unsigned char *drizzle_pack_datetime(drizzle_datetime_st *datetime, unsigned char *ptr)
{
  uint8_t length= 0;

  if (datetime->microsecond)
  {
    drizzle_set_byte4(ptr+8, datetime->microsecond);
    length = 11;
  }

  if (length || datetime->hour || datetime->minute || datetime->second)
  {
    drizzle_set_byte1(ptr+5, datetime->hour);
    drizzle_set_byte1(ptr+6, datetime->minute);
    drizzle_set_byte1(ptr+7, datetime->second);
    /* If only date+time is provided then we are packing 7 bytes */
    if (!length)
      length = 7;
  }

  if (length || datetime->year || datetime->month || datetime->day)
  {
    drizzle_set_byte2(ptr+1, datetime->year);
    drizzle_set_byte1(ptr+3, datetime->month);
    drizzle_set_byte1(ptr+4, datetime->day);
    /* If only date is provided then we are packing 4 bytes */
    if (!length)
      length = 4;
  }

  /* If nothing is set then we are sending a 0 length datetime */

  drizzle_set_byte1(ptr, length);
  return ptr + 1 + length;
}

void drizzle_unpack_time(drizzle_field_t field, size_t length, drizzle_datetime_st *datetime, uint8_t decimals)
{
  memset(datetime, 0, sizeof(*datetime));

  if (length)
  {
    datetime->negative= field[0];
    datetime->day= drizzle_get_byte4(&field[1]);
    datetime->hour= field[5];
    datetime->minute= field[6];
    datetime->second= field[7];
    if (length > 8)
    {
      datetime->microsecond= drizzle_get_byte4(&field[8]);
    }
    if (decimals)
    {
      datetime->show_microseconds= true;
    }
  }
}

void drizzle_unpack_datetime(drizzle_field_t field, size_t length, drizzle_datetime_st *datetime, uint8_t decimals)
{
  memset(datetime, 0, sizeof(*datetime));

  if (length)
  {
    datetime->negative= false;
    datetime->year= drizzle_get_byte2(field);
    datetime->month= field[2];
    datetime->day= field[3];
    if (length > 4)
    {
      datetime->hour= field[4];
      datetime->minute= field[5];
      datetime->second= field[6];
      if (length > 7)
      {
        datetime->microsecond= drizzle_get_byte4(&field[7]);
      }
    }
    if (decimals)
    {
      datetime->show_microseconds= true;
    } 
  }
}

drizzle_return_t drizzle_unpack_string(drizzle_st *con, char *buffer,
                                       size_t max_length)
{
  drizzle_return_t ret= DRIZZLE_RETURN_OK;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  uint64_t length= drizzle_unpack_length(con, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    if (ret == DRIZZLE_RETURN_NULL_SIZE)
    {
      drizzle_set_error(con, "drizzle_unpack_string",
                        "unexpected NULL length");
    }

    return ret;
  }

  if (length > con->packet_size)
  {
    drizzle_set_error(con, "drizzle_unpack_string",
                           "string extends past end of packet");
    return DRIZZLE_RETURN_UNEXPECTED_DATA;
  }
  if (length > con->buffer_size)
  {
    return DRIZZLE_RETURN_IO_WAIT;
  }

  assert(max_length > 1);
  if (length < max_length)
  {
    if (length > 0)
      memcpy(buffer, con->buffer_ptr, (size_t)length);

    buffer[length]= 0;
  }
  else
  {
    memcpy(buffer, con->buffer_ptr, max_length - 1);
    buffer[max_length - 1]= 0;
  }

  con->buffer_ptr+= length;
  con->buffer_size-= length;
  con->packet_size-= (uint32_t)length;

  return DRIZZLE_RETURN_OK;
}

unsigned char *drizzle_pack_auth(drizzle_st *con, unsigned char *ptr,
                           drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused_ret;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused_ret;
  }

  if (con == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return NULL;
  }

  if (con->user[0] != 0)
  {
    memcpy(ptr, con->user, strlen(con->user));
    ptr+= strlen(con->user);
  }

  ptr[0]= 0;
  ptr++;

  if (con->options.raw_scramble && con->scramble != NULL)
  {
    ptr[0]= DRIZZLE_MAX_SCRAMBLE_SIZE;
    ptr++;

    memcpy(ptr, con->scramble, DRIZZLE_MAX_SCRAMBLE_SIZE);
    ptr+= DRIZZLE_MAX_SCRAMBLE_SIZE;
  }
  else if (con->password[0] == 0)
  {
    ptr[0]= 0;
    ptr++;
    con->packet_size-= DRIZZLE_MAX_SCRAMBLE_SIZE;
  }
  else
  {
    ptr[0]= DRIZZLE_MAX_SCRAMBLE_SIZE;
    ptr++;

    if (con->options.auth_plugin)
    {
      snprintf((char *)ptr, DRIZZLE_MAX_SCRAMBLE_SIZE, "%s", con->password);
      ptr[DRIZZLE_MAX_SCRAMBLE_SIZE-1]= 0;
    }
    else
    {
      *ret_ptr= _pack_scramble_hash(con, ptr);
      if (*ret_ptr != DRIZZLE_RETURN_OK)
        return ptr;
    }

    ptr+= DRIZZLE_MAX_SCRAMBLE_SIZE;
  }

  if (con->db[0] != 0)
  {
    memcpy(ptr, con->db, strlen(con->db));
    ptr+= strlen(con->db);
  }

  ptr[0]= 0;
  ptr++;

  *ret_ptr= DRIZZLE_RETURN_OK;
  return ptr;
}

/*
 * Private definitions
 */

static drizzle_return_t _pack_scramble_hash(drizzle_st *con,
                                            unsigned char *buffer)
{
  SHA1_CTX ctx;
  unsigned char hash_tmp1[SHA1_DIGEST_LENGTH];
  unsigned char hash_tmp2[SHA1_DIGEST_LENGTH];

  if (SHA1_DIGEST_LENGTH != DRIZZLE_MAX_SCRAMBLE_SIZE)
  {
    drizzle_set_error(con, "_pack_scramble_hash",
                      "SHA1 hash size mismatch:%u:%u", SHA1_DIGEST_LENGTH,
                      DRIZZLE_MAX_SCRAMBLE_SIZE);
    return DRIZZLE_RETURN_INTERNAL_ERROR;
  }

  if (con->scramble == NULL)
  {
    drizzle_set_error(con, "_pack_scramble_hash",
                      "no scramble buffer");
    return DRIZZLE_RETURN_NO_SCRAMBLE;
  }

  /* First hash the password. */
  SHA1Init(&ctx);
  SHA1Update(&ctx, (unsigned char *)(con->password), strlen(con->password));
  SHA1Final(hash_tmp1, &ctx);

  /* Second, hash the password hash. */
  SHA1Init(&ctx);
  SHA1Update(&ctx, hash_tmp1, SHA1_DIGEST_LENGTH);
  SHA1Final(hash_tmp2, &ctx);

  /* Third, hash the scramble and the double password hash. */
  SHA1Init(&ctx);
  SHA1Update(&ctx, con->scramble, SHA1_DIGEST_LENGTH);
  SHA1Update(&ctx, hash_tmp2, SHA1_DIGEST_LENGTH);
  SHA1Final(buffer, &ctx);

  /* Fourth, xor the last hash against the first password hash. */
  uint32_t x;
  for (x= 0; x < SHA1_DIGEST_LENGTH; x++)
  {
    buffer[x]= buffer[x] ^ hash_tmp1[x];
  }

  return DRIZZLE_RETURN_OK;
}
