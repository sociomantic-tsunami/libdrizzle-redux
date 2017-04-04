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
 * @brief Result definitions
 */

#include "config.h"
#include "src/common.h"

/*
 * Common definitions
 */

drizzle_result_st *drizzle_result_create(drizzle_st *con)
{
  drizzle_result_st *result;
  if (con == NULL)
  {
    return NULL;
  }

  result= new (std::nothrow) drizzle_result_st;
  if (result == NULL)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "Failed to allocate.");
    return NULL;
  }

  result->con= con;
  con->result= result;

  if (con->result_list)
    con->result_list->prev= result;
  result->next= con->result_list;
  con->result_list= result;
  con->result_count++;

  return result;
}

void drizzle_result_free(drizzle_result_st *result)
{
  drizzle_column_st* column;
  int64_t y;

  if (result == NULL)
  {
    return;
  }

  for (column= result->column_list; column != NULL; column= result->column_list)
  {
    drizzle_column_free(column);
  }

  delete[] result->column_buffer;

  if (result->options & DRIZZLE_RESULT_BUFFER_ROW)
  {
    uint64_t x;

    for (x= 0; x < result->row_count; x++)
    {
      for (y= 0; y < (result->column_count - result->null_bitcount); y++)
      {
        delete[] result->row_list[x][y];
      }
      delete[] result->row_list[x];
      if (result->null_bitmap_list != NULL)
      {
        delete[] result->null_bitmap_list[x];
      }
      delete[] result->field_sizes_list[x];
    }
    if (result->null_bitmap_list != NULL)
    {
      free(result->null_bitmap_list);
    }
    free(result->row_list);
    free(result->field_sizes_list);
  }

  if (result->field_buffer)
  {
    for (y= 0; y < (result->column_count); y++)
    {
      free(result->field_buffer[y]);
    }
    delete[] result->field_buffer;
    delete[] result->field_buffer_sizes;
  }
  delete[] result->row;

  if (result->con)
  {
    result->con->result_count--;
    if (result->con->result_list == result)
      result->con->result_list= result->next;
  }

  if (result->prev)
  {
    result->prev->next= result->next;
  }
  if (result->next)
  {
    result->next->prev= result->prev;
  }

  delete result;
}

void drizzle_result_free_all(drizzle_st *con)
{
  if (con == NULL)
  {
    return;
  }

  while (con->result_list != NULL)
  {
    drizzle_result_free(con->result_list);
  }
}

drizzle_st *drizzle_result_drizzle_con(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  return result->con;
}

bool drizzle_result_eof(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return false;
  }

  return result->options & DRIZZLE_RESULT_EOF_PACKET;
}

const char *drizzle_result_message(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  return result->info;
}

uint16_t drizzle_result_error_code(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->error_code;
}

const char *drizzle_result_sqlstate(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  return result->sqlstate;
}

uint16_t drizzle_result_warning_count(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->warning_count;
}

uint64_t drizzle_result_insert_id(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->insert_id;
}

uint64_t drizzle_result_affected_rows(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->affected_rows;
}

uint16_t drizzle_result_column_count(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->column_count;
}

uint64_t drizzle_result_row_count(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->row_count;
}

/*
 * Client definitions
 */

drizzle_result_st *drizzle_result_read(drizzle_st *con,
                                       drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused;
  }

  if (con == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return NULL;
  }

  if (con->has_state())
  {
    con->result= drizzle_result_create(con);
    if (con->result == NULL)
    {
      *ret_ptr= DRIZZLE_RETURN_MEMORY;
      return NULL;
    }

    con->push_state(drizzle_state_result_read);
    con->push_state(drizzle_state_packet_read);
  }

  *ret_ptr= drizzle_state_loop(con);
  return con->result;
}

drizzle_return_t drizzle_result_buffer(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_return_t ret;
  drizzle_row_t row;
  drizzle_row_t *row_list;
  size_t **field_sizes_list;

  if (!(result->options & DRIZZLE_RESULT_BUFFER_COLUMN))
  {
    ret= drizzle_column_buffer(result);
    if (ret != DRIZZLE_RETURN_OK)
      return ret;
  }

  if (result->column_count == 0)
  {
    result->options = (drizzle_result_options_t)((int)result->options | (int)DRIZZLE_RESULT_BUFFER_ROW);
    return DRIZZLE_RETURN_OK;
  }

  while (1)
  {
    uint16_t x;
    row= drizzle_row_buffer(result, &ret);
    if (ret != DRIZZLE_RETURN_OK)
      return ret;

    if (row == NULL)
      break;

    if (result->row_list_size < result->row_count)
    {
      size_t new_row_list_size = result->row_list_size + DRIZZLE_ROW_GROW_SIZE;

      row_list= (drizzle_row_t *)realloc(result->row_list, sizeof(drizzle_row_t) * new_row_list_size);
      if (row_list == NULL)
      {
        drizzle_row_free(result, row);
        drizzle_set_error(result->con, __FILE_LINE_FUNC__, "Failed to realloc row_list.");
        return DRIZZLE_RETURN_MEMORY;
      }
      result->row_list= row_list;

      if (result->binary_rows)
      {
        uint8_t **null_bitmap_list= (uint8_t **)realloc(result->null_bitmap_list, sizeof(uint8_t *) * new_row_list_size);
        if (null_bitmap_list == NULL)
        {
          drizzle_row_free(result, row);
          drizzle_set_error(result->con, __FILE_LINE_FUNC__, "Failed to realloc null_bitmap_list.");
          return DRIZZLE_RETURN_MEMORY;
        }
        result->null_bitmap_list= null_bitmap_list;
      }

      field_sizes_list= (size_t **)realloc(result->field_sizes_list, sizeof(size_t *) * new_row_list_size);
      if (field_sizes_list == NULL)
      {
        drizzle_row_free(result, row);
        drizzle_set_error(result->con, __FILE_LINE_FUNC__, "Failed to realloc field list.");
        return DRIZZLE_RETURN_MEMORY;
      }
      result->field_sizes_list= field_sizes_list;

      result->row_list_size= new_row_list_size;
    }

    if (result->binary_rows)
    {
      result->null_bitmap_list[result->row_current - 1]= result->null_bitmap;
    }

    result->row_list[result->row_current - 1]= new drizzle_field_t[result->column_count]();
    for (x= 0; x < result->column_count; x++)
    {
      if (result->field_sizes[x] > 0)
      {
        result->row_list[result->row_current - 1][x]= new char[result->field_sizes[x]+1]();
        memcpy(result->row_list[result->row_current - 1][x], row[x], result->field_sizes[x]);
      }
    }
    result->field_sizes_list[result->row_current - 1]= result->field_sizes;
  }

  result->options = (drizzle_result_options_t)((int)result->options | (int)DRIZZLE_RESULT_BUFFER_ROW);
  return DRIZZLE_RETURN_OK;
}

size_t drizzle_result_row_size(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->con->packet_size;
}

/*
 * Internal state functions.
 */

drizzle_return_t drizzle_state_result_read(drizzle_st *con)
{
  drizzle_return_t ret;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  __LOG_LOCATION__

  /* Assume the entire result packet will fit in the buffer. */
  if (con->buffer_size < con->packet_size)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (con->buffer_ptr[0] == 0)
  {
    con->buffer_ptr++;
    /* We can ignore the returns since we've buffered the entire packet. */
    if (con->command == DRIZZLE_COMMAND_STMT_PREPARE)
    {
      con->stmt->id= drizzle_get_byte4(con->buffer_ptr);
      con->result->column_count= drizzle_get_byte2(con->buffer_ptr + 4);
      con->stmt->param_count= drizzle_get_byte2(con->buffer_ptr + 6);
      con->result->warning_count= drizzle_get_byte2(con->buffer_ptr + 9);
      con->buffer_ptr+= 11;
      con->buffer_size-= 12;
      con->packet_size-= 12;
    }
    else
    {
      con->result->affected_rows= drizzle_unpack_length(con, &ret);
      con->result->insert_id= drizzle_unpack_length(con, &ret);
      con->status= (drizzle_status_t)drizzle_get_byte2(con->buffer_ptr);
      con->result->warning_count= drizzle_get_byte2(con->buffer_ptr +2);
      con->buffer_ptr+= 4;
      con->buffer_size-= 5;
      con->packet_size-= 5;
    }
    if (con->packet_size > 0)
    {
      /* Skip one byte for message size. */
      con->buffer_ptr+= 1;
      con->buffer_size-= 1;
      con->packet_size-= 1;
    }
    ret= DRIZZLE_RETURN_OK;
  }
  else if (con->buffer_ptr[0] == 254)
  {
    con->result->options= DRIZZLE_RESULT_EOF_PACKET;
    con->result->warning_count= drizzle_get_byte2(con->buffer_ptr + 1);
    con->status= (drizzle_status_t)drizzle_get_byte2(con->buffer_ptr + 3);
    con->buffer_ptr+= 5;
    con->buffer_size-= 5;
    con->packet_size-= 5;
    ret= DRIZZLE_RETURN_OK;
  }
  else if (drizzle_check_unpack_error(con))
  {
    memcpy(con->result->sqlstate, con->sqlstate,
           DRIZZLE_MAX_SQLSTATE_SIZE);
    con->result->sqlstate[DRIZZLE_MAX_SQLSTATE_SIZE]= 0;
    ret= DRIZZLE_RETURN_ERROR_CODE;
  }
  else
  {
    /* We can ignore the return since we've buffered the entire packet. */
    con->result->column_count= (uint16_t)drizzle_unpack_length(con, &ret);
    ret= DRIZZLE_RETURN_OK;
  }

  if (con->packet_size > 0)
  {
    snprintf(con->last_error, DRIZZLE_MAX_ERROR_SIZE, "%.*s",
             (int)con->packet_size, con->buffer_ptr);
    con->last_error[DRIZZLE_MAX_ERROR_SIZE-1]= 0;
    snprintf(con->result->info, DRIZZLE_MAX_INFO_SIZE, "%.*s",
             (int)con->packet_size, con->buffer_ptr);
    con->result->info[DRIZZLE_MAX_INFO_SIZE-1]= 0;
    con->buffer_ptr+= con->packet_size;
    con->buffer_size-= con->packet_size;
    con->packet_size= 0;
  }

  con->pop_state();
  return ret;
}
