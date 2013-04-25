/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
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
 * @brief Row definitions
 */

#include "config.h"
#include "libdrizzle/common.h"

/*
 * Client definitions
 */

uint64_t drizzle_row_read(drizzle_result_st *result, drizzle_return_t *ret_ptr)
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

  if ((result->column_current != result->column_count) && (!(result->options & DRIZZLE_RESULT_BUFFER_COLUMN)))
  {
    drizzle_set_error(result->con, "drizzle_row_read", "cannot retrieve rows until all columns are retrieved");
    *ret_ptr= DRIZZLE_RETURN_NOT_READY;
    return 0;
  }

  if (result->has_state())
  {
    result->push_state(drizzle_state_row_read);
    result->push_state(drizzle_state_packet_read);
  }

  *ret_ptr= drizzle_state_loop(result->con);

  return result->row_current;
}

drizzle_row_t drizzle_row_buffer(drizzle_result_st *result,
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

  size_t total;
  drizzle_field_t field;
  drizzle_row_t row;

  if (result->row == NULL)
  {
    if (drizzle_row_read(result, ret_ptr) == 0 || *ret_ptr != DRIZZLE_RETURN_OK)
    {
      return NULL;
    }

    result->row= new (std::nothrow) drizzle_field_t[result->column_count];
    if (result->row == NULL)
    {
      drizzle_set_error(result->con, __func__, "Failed to allocate.");
      *ret_ptr= DRIZZLE_RETURN_MEMORY;
      return NULL;
    }

    result->field_sizes= new (std::nothrow) size_t[result->column_count];
    if (result->field_sizes == NULL)
    {
      drizzle_set_error(result->con, __func__, "Failed to allocate.");
      *ret_ptr= DRIZZLE_RETURN_MEMORY;
      return NULL;
    }
  }

  while (1)
  {
    field= drizzle_field_buffer(result, &total, ret_ptr);
    if (*ret_ptr == DRIZZLE_RETURN_ROW_END)
      break;

    if (*ret_ptr != DRIZZLE_RETURN_OK)
    {
      if (*ret_ptr != DRIZZLE_RETURN_IO_WAIT)
      {
        delete[] result->row;
        delete[] result->field_sizes;
        result->row= NULL;
        result->field_sizes= NULL;
      }

      return NULL;
    }

    result->row[result->field_current - 1]= field;
    result->field_sizes[result->field_current - 1]= total;
  }

  *ret_ptr= DRIZZLE_RETURN_OK;
  row= result->row;
  result->row= NULL;

  return row;
}

void drizzle_row_free(drizzle_result_st *result, drizzle_row_t row)
{
  uint16_t x;

  if (result == NULL)
  {
    return;
  }

  for (x= 0; x < (result->column_count - result->null_bitcount); x++)
  {
    drizzle_field_free(row[x]);
  }

  if (!(result->options & DRIZZLE_RESULT_BUFFER_ROW))
  { 
    delete[] result->null_bitmap;
    result->null_bitmap= NULL;
    delete[] result->field_sizes;
    result->field_sizes= NULL;
  } 

  delete[] row;
}

size_t *drizzle_row_field_sizes(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  return result->field_sizes;
}

drizzle_row_t drizzle_row_next(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  if (result->row_current == result->row_count)
  {
    return NULL;
  }

  result->field_sizes= result->field_sizes_list[result->row_current];
  if (result->binary_rows)
  {
    result->null_bitmap= result->null_bitmap_list[result->row_current];
  }
  result->row_current++;
  return result->row_list[result->row_current - 1];
}

drizzle_row_t drizzle_row_prev(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  if (result->row_current == 0)
    return NULL;

  result->row_current--;
  result->field_sizes= result->field_sizes_list[result->row_current];
  if (result->binary_rows)
  {
    result->null_bitmap= result->null_bitmap_list[result->row_current];
  }
  return result->row_list[result->row_current];
}

void drizzle_row_seek(drizzle_result_st *result, uint64_t row)
{
  if (result == NULL)
  {
    return;
  }

  if (row <= result->row_count)
    result->row_current= row;
}

drizzle_row_t drizzle_row_index(drizzle_result_st *result, uint64_t row)
{
  if (result == NULL)
  {
    return NULL;
  }

  if (row >= result->row_count)
    return NULL;

  return result->row_list[row];
}

uint64_t drizzle_row_current(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->row_current;
}

/*
 * Internal state functions.
 */

drizzle_return_t drizzle_state_row_read(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_log_debug(con, "drizzle_state_row_read");

  if (con->packet_size != 0 && con->buffer_size < con->packet_size && 
    con->buffer_size < 5)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (con->packet_size == 5 && con->buffer_ptr[0] == 254)
  {
    /* Got EOF packet, no more rows. */
    con->result->row_current= 0;
    con->result->warning_count= drizzle_get_byte2(con->buffer_ptr + 1);
    con->status= (drizzle_status_t)drizzle_get_byte2(con->buffer_ptr + 3);
    con->buffer_ptr+= 5;
    con->buffer_size-= 5;
  }
  else if (con->buffer_ptr[0] == 255)
  {
    con->pop_state();
    con->push_state(drizzle_state_result_read);
    return DRIZZLE_RETURN_OK;
  }
  else if (con->result->options & DRIZZLE_RESULT_ROW_BREAK)
  {
    con->result->options = (drizzle_result_options_t)((int)con->result->options & (int)~DRIZZLE_RESULT_ROW_BREAK);
  }
  else
  {
    con->result->row_count++;
    con->result->row_current++;
    con->result->field_current= 0;
  }

  con->pop_state();
  return DRIZZLE_RETURN_OK;
}

