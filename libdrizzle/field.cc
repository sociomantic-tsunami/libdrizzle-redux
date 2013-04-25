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
 * @brief Field definitions
 */

#include "config.h"
#include "libdrizzle/common.h"

#include <inttypes.h>

/*
 * Client definitions
 */

drizzle_field_t drizzle_field_read(drizzle_result_st *result, uint64_t *offset,
                                   size_t *size, uint64_t *total,
                                   drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused_ret;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused_ret;
  }

  if (result == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return 0;
  }

  if (result->has_state())
  {
    if (result->field_current == (result->column_count - result->null_bitcount))
    {
      *ret_ptr= DRIZZLE_RETURN_ROW_END;
      return NULL;
    }

    if (result->binary_rows)
    {
      result->push_state(drizzle_state_binary_field_read);
    }
    else
    {
      result->push_state(drizzle_state_field_read);
    }
  }

  if (result->binary_rows && (result->field_current == 0))
  {
    result->push_state(drizzle_state_binary_null_read);
  }

  *ret_ptr= drizzle_state_loop(result->con);
  if (*ret_ptr == DRIZZLE_RETURN_OK &&
      result->options & DRIZZLE_RESULT_ROW_BREAK)
  {
    *ret_ptr= DRIZZLE_RETURN_ROW_BREAK;
  }

  if (offset)
  {
    *offset= result->field_offset;
  }

  if (size)
  {
    *size= result->field_size;
  }

  if (total)
  {
    *total= result->field_total;
  }

  return result->field;
}

drizzle_field_t drizzle_field_buffer(drizzle_result_st *result, size_t *total,
                                     drizzle_return_t *ret_ptr)
{
  uint64_t offset= 0;
  size_t size= 0;
  uint64_t wire_size;

  drizzle_return_t unused_ret;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused_ret;
  }

  if (result == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return 0;
  }

  drizzle_field_t field= drizzle_field_read(result, &offset, &size, &wire_size, ret_ptr);

  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return NULL;
  }

  if (field == NULL)
  {
    *total= 0;
    return NULL;
  }

  if ((SIZE_MAX < UINT64_MAX) && (wire_size >= SIZE_MAX))
  {
    drizzle_set_error(result->con, __func__, "Field is larger than memory.");
    *ret_ptr= DRIZZLE_RETURN_MEMORY;
    return NULL;
  }
  *total = (size_t)wire_size;
  
  if (result->field_buffer == NULL)
  {
    result->field_buffer= new (std::nothrow) char[(*total) + 1];
    if (result->field_buffer == NULL)
    {
      drizzle_set_error(result->con, __func__, "Failed to allocate.");
      *ret_ptr= DRIZZLE_RETURN_MEMORY;
      return NULL;
    }
  }

  memcpy(result->field_buffer + offset, field, size);

  while ((offset + size) != (*total))
  {
    field= drizzle_field_read(result, &offset, &size, &wire_size, ret_ptr);
    if (*ret_ptr != DRIZZLE_RETURN_OK)
    {
      return NULL;
    }
    assert(wire_size == (uint64_t)*total);
    
    memcpy(result->field_buffer + offset, field, size);
  }

  field= result->field_buffer;
  result->field_buffer= NULL;
  field[*total]= 0;

  return field;
}

void drizzle_field_free(drizzle_field_t field)
{
  if (field)
  {
    delete[] field;
  }
}

/*
 * Internal state functions.
 */

drizzle_return_t drizzle_state_field_read(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  drizzle_log_debug(con, "drizzle_state_field_read");

  if (con->buffer_size == 0)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  con->result->field_offset+= con->result->field_size;
  if (con->result->field_offset == con->result->field_total)
  {
    con->result->field_offset= 0;
    con->result->field_size= 0;

    drizzle_return_t ret;
    con->result->field_total= drizzle_unpack_length(con, &ret);
    if (ret == DRIZZLE_RETURN_NULL_SIZE)
    {
      con->result->field= NULL;
      con->result->field_current++;
      con->pop_state();
      return DRIZZLE_RETURN_OK;
    }
    else if (ret != DRIZZLE_RETURN_OK)
    {
      if (ret == DRIZZLE_RETURN_IO_WAIT)
      {
        con->push_state(drizzle_state_read);
        return DRIZZLE_RETURN_OK;
      }

      return ret;
    }

    drizzle_log_debug(con,
                      "field_offset= %"PRIu64", field_size= %zu, field_total= %"PRIu64,
                      con->result->field_offset, con->result->field_size,
                      con->result->field_total);

    uint32_t available_data = (con->buffer_size < con->packet_size)? (uint32_t)con->buffer_size : con->packet_size;
    /* packet_size fits in uint32, so available_data fits in uint32 */
    
    if (available_data >= con->result->field_total)
    {
      /* narrowing cast is safe because field_total<packet_size and packet_size is uint32 */
      con->result->field_size= (uint32_t)con->result->field_total;
    }
    else
    {
      con->result->field_size= available_data;
    }
  }
  else
  {
    uint32_t available_data = (con->buffer_size < con->packet_size)? (uint32_t)con->buffer_size : con->packet_size;
    /* packet_size fits in uint32, so available_data fits in uint32 */
    
    uint64_t field_remaining = con->result->field_total - con->result->field_offset;
    
    if (field_remaining <= available_data)
    {
      /* narrowing cast is safe because field_remaining<=packet_size and packet_size is uint32 */
      con->result->field_size= (uint32_t)field_remaining;
    }
    else
    {
      con->result->field_size= available_data;
    }
  }

  /* This is a special case when a row is larger than the packet size. */
  if (con->result->field_size > con->packet_size)
  {
    con->result->field_size= con->packet_size;

    if (con->state.raw_packet)
    {
      con->result->options = (drizzle_result_options_t)((int)con->result->options | DRIZZLE_RESULT_ROW_BREAK);
    }
    else
    {
      con->push_state(drizzle_state_packet_read);
    }
  }

  con->result->field= (char *)con->buffer_ptr;
  con->buffer_ptr+= con->result->field_size;
  con->buffer_size-= con->result->field_size;
  con->packet_size-= con->result->field_size;

  drizzle_log_debug(con,
                    "field_offset= %"PRIu64", field_size= %zu, field_total= %"PRIu64,
                    con->result->field_offset, con->result->field_size,
                    con->result->field_total);

  if ((con->result->field_offset + con->result->field_size) ==
      con->result->field_total)
  {
    if (con->result->column_buffer != NULL &&
        con->result->column_buffer[con->result->field_current].size <
        con->result->field_total)
    {
      con->result->column_buffer[con->result->field_current].size=
                                                       con->result->field_total;
    }

    con->result->field_current++;
  }

  if (con->result->field_total == 0 || con->result->field_size > 0 ||
      con->packet_size == 0)
  {
    con->pop_state();
  }

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_binary_null_read(drizzle_st *con)
{
  uint16_t bit_count= 0;
  con->result->null_bitmap_length= (con->result->column_count+7+2)/8;
  con->result->null_bitmap= new uint8_t[con->result->null_bitmap_length];
  con->buffer_ptr++;

  memcpy(con->result->null_bitmap, con->buffer_ptr, con->result->null_bitmap_length);

  for (uint16_t it= 0; it < con->result->null_bitmap_length; it++)
  {
    uint8_t data= con->result->null_bitmap[it];
    while (data)
    {
      data &= (data - 1);
      bit_count++;
    }
  }
  con->result->null_bitcount = bit_count;
  con->buffer_ptr+= con->result->null_bitmap_length;
  con->buffer_size-= con->result->null_bitmap_length+1;
  con->packet_size-= con->result->null_bitmap_length+1;

  con->pop_state();

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_binary_field_read(drizzle_st *con)
{
  drizzle_return_t ret;

  switch(con->result->column_buffer[con->result->field_current].type)
  {
    case DRIZZLE_COLUMN_TYPE_NULL:
      break;
    case DRIZZLE_COLUMN_TYPE_TINY:
      con->result->field_size= 1;
      break;
    case DRIZZLE_COLUMN_TYPE_SHORT:
    case DRIZZLE_COLUMN_TYPE_YEAR:
      con->result->field_size= 2;
      break;
    case DRIZZLE_COLUMN_TYPE_INT24:
    case DRIZZLE_COLUMN_TYPE_LONG:
    case DRIZZLE_COLUMN_TYPE_FLOAT:
      con->result->field_size= 4;
      break;
    case DRIZZLE_COLUMN_TYPE_LONGLONG:
    case DRIZZLE_COLUMN_TYPE_DOUBLE:
      con->result->field_size= 8;
      break;
    case DRIZZLE_COLUMN_TYPE_TIME:
    case DRIZZLE_COLUMN_TYPE_DATE:
    case DRIZZLE_COLUMN_TYPE_DATETIME:
    case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
    case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
    case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
    case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
    case DRIZZLE_COLUMN_TYPE_BLOB:
    case DRIZZLE_COLUMN_TYPE_BIT:
    case DRIZZLE_COLUMN_TYPE_STRING:
    case DRIZZLE_COLUMN_TYPE_VAR_STRING:
    case DRIZZLE_COLUMN_TYPE_DECIMAL:
    case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
    case DRIZZLE_COLUMN_TYPE_NEWDATE:
      con->result->field_size= (size_t)drizzle_unpack_length(con, &ret);
      if (ret != DRIZZLE_RETURN_OK)
      {
        return ret;
      }
      break;
    case DRIZZLE_COLUMN_TYPE_VARCHAR:
    case DRIZZLE_COLUMN_TYPE_ENUM:
    case DRIZZLE_COLUMN_TYPE_SET:
    case DRIZZLE_COLUMN_TYPE_GEOMETRY:
    /* TODO: Next 3 we really need to support */
    case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
    case DRIZZLE_COLUMN_TYPE_DATETIME2:
    case DRIZZLE_COLUMN_TYPE_TIME2:
    default:
      return DRIZZLE_RETURN_UNEXPECTED_DATA;
      break;
  }

  con->result->field= (char*) con->buffer_ptr;
  con->buffer_ptr+= con->result->field_size;
  con->buffer_size-= con->result->field_size;
  con->packet_size-= con->result->field_size;
  con->result->field_total= con->result->field_size;

  con->result->field_current++;
  con->pop_state();
  return DRIZZLE_RETURN_OK;
}

