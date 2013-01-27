/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
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
 * @brief Column definitions
 */

#include "config.h"
#include "libdrizzle/common.h"

/*
 * Common definitions
 */

drizzle_column_st *drizzle_column_create(drizzle_result_st *result)
{
  drizzle_column_st *column;
  if (result == NULL)
  {
    return NULL;
  }

  column= new (std::nothrow) drizzle_column_st;
  if (column == NULL)
  {
    drizzle_set_error(result->con, __func__, "Failed to allocate.");
    return NULL;
  }

  column->result= result;

  if (result->column_list)
    result->column_list->prev= column;

  column->next= result->column_list;
  result->column_list= column;

  return column;
}

void drizzle_column_free(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return;
  }

  if (column->result->column_list == column)
    column->result->column_list= column->next;

  if (column->prev)
    column->prev->next= column->next;

  if (column->next)
    column->next->prev= column->prev;

  delete column;
}

drizzle_result_st *drizzle_column_drizzle_result(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return NULL;
  }

  return column->result;
}

const char *drizzle_column_catalog(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return NULL;
  }

  return column->catalog;
}

const char *drizzle_column_db(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return NULL;
  }

  return column->db;
}

const char *drizzle_column_table(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return NULL;
  }

  return column->table;
}

const char *drizzle_column_orig_table(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return NULL;
  }

  return column->orig_table;
}

const char *drizzle_column_name(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return NULL;
  }

  return column->name;
}

const char *drizzle_column_orig_name(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return NULL;
  }

  return column->orig_name;
}

drizzle_charset_t drizzle_column_charset(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return DRIZZLE_CHARSET_NONE;
  }

  return column->charset;
}

uint32_t drizzle_column_size(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return 0;
  }

  return column->size;
}

size_t drizzle_column_max_size(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return 0;
  }

  return column->max_size;
}

drizzle_column_type_t drizzle_column_type(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return DRIZZLE_COLUMN_TYPE_DECIMAL;
  }

  return column->type;
}

drizzle_column_flags_t drizzle_column_flags(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return DRIZZLE_COLUMN_FLAGS_NONE;
  }

  return drizzle_column_flags_t(column->flags);
}

uint8_t drizzle_column_decimals(drizzle_column_st *column)
{
  if (column == NULL)
  {
    return 0;
  }

  return column->decimals;
}

const unsigned char *drizzle_column_default_value(drizzle_column_st *column,
                                            size_t *size)
{
  if (column == NULL)
  {
    return NULL;
  }

  *size= column->default_value_size;
  return column->default_value;
}

/*
 * Client definitions
 */

drizzle_return_t drizzle_column_skip(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_return_t ret;
  if (result->has_state())
  {
    result->options = (drizzle_result_options_t)((int)result->options | (int)DRIZZLE_RESULT_SKIP_COLUMN);

    result->push_state(drizzle_state_column_read);
    result->push_state(drizzle_state_packet_read);
  }
  ret= drizzle_state_loop(result->con);
  result->options = (drizzle_result_options_t)((int)result->options & (int)~DRIZZLE_RESULT_SKIP_COLUMN);
  return ret;
}

drizzle_return_t drizzle_column_skip_all(drizzle_result_st *result)
{
  uint16_t it;
  if (result == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  for (it= 1; it <= result->column_count; it++)
  {
    drizzle_return_t ret= drizzle_column_skip(result);

    if (ret != DRIZZLE_RETURN_OK)
    {
      return ret;
    }
  }

  return DRIZZLE_RETURN_OK;
}

drizzle_column_st *drizzle_column_read(drizzle_result_st *result,
                                       drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused_ret;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused_ret;
  }

  if (result == NULL)
  {
    return NULL;
  }

  if (result->has_state())
  {
    result->push_state(drizzle_state_column_read);
    result->push_state(drizzle_state_packet_read);
  }

  *ret_ptr= drizzle_state_loop(result->con);

  return result->column;
}

drizzle_return_t drizzle_column_buffer(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_return_t ret;

  if (result->column_buffer == NULL)
  {
    if (result->column_count == 0)
    {
      result->options = (drizzle_result_options_t)((int)result->options | (int)DRIZZLE_RESULT_BUFFER_COLUMN);
      return DRIZZLE_RETURN_OK;
    }

    result->column_buffer= new (std::nothrow) drizzle_column_st[result->column_count];
    if (result->column_buffer == NULL)
    {
      drizzle_set_error(result->con, __func__, "Failed to allocate.");

      return DRIZZLE_RETURN_MEMORY;
    }
  }

  /* No while body, just keep calling to buffer columns. */
  do
  {
    result->column= &(result->column_buffer[result->column_current]);
  }
  while(drizzle_column_read(result, &ret) != NULL && ret == DRIZZLE_RETURN_OK);

  if (ret == DRIZZLE_RETURN_OK)
  {
    result->column_current= 0;
    result->options = (drizzle_result_options_t)((int)result->options | (int)DRIZZLE_RESULT_BUFFER_COLUMN);
  }

  return ret;
}

drizzle_column_st *drizzle_column_next(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  if (result->column_current == result->column_count)
  {
    return NULL;
  }

  result->column_current++;
  return &(result->column_buffer[result->column_current - 1]);
}

drizzle_column_st *drizzle_column_prev(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return NULL;
  }

  if (result->column_current == 0)
  {
    return NULL;
  }

  result->column_current--;
  return &(result->column_buffer[result->column_current]);
}

void drizzle_column_seek(drizzle_result_st *result, uint16_t column)
{
  if (result == NULL)
  {
    return;
  }

  if (column <= result->column_count)
  {
    result->column_current= column;
  }
}

drizzle_column_st *drizzle_column_index(drizzle_result_st *result,
                                        uint16_t column)
{
  if (result == NULL)
  {
    return NULL;
  }

  if (column >= result->column_count)
  {
    return NULL;
  }

  return &(result->column_buffer[column]);
}

uint16_t drizzle_column_current(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return 0;
  }

  return result->column_current;
}

/*
 * Server definitions
 */

void drizzle_column_set_default_value(drizzle_column_st *column,
                                      const unsigned char *default_value,
                                      size_t size)
{
  if (column == NULL)
  {
    return;
  }

  if (default_value == NULL)
  {
    column->default_value[0]= 0;
  }
  else
  {
    if (size < DRIZZLE_MAX_DEFAULT_VALUE_SIZE)
    {
      memcpy(column->default_value, default_value, size);
      column->default_value[size]= 0;
      column->default_value_size= size;
    }
    else
    {
      memcpy(column->default_value, default_value,
             DRIZZLE_MAX_DEFAULT_VALUE_SIZE - 1);
      column->default_value[DRIZZLE_MAX_DEFAULT_VALUE_SIZE - 1]= 0;
      column->default_value_size= DRIZZLE_MAX_DEFAULT_VALUE_SIZE;
    }
  }
}

/*
 * Internal state functions.
 */

drizzle_return_t drizzle_state_column_read(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_column_st *column;

  drizzle_log_debug(con, "drizzle_state_column_read");

  /* Assume the entire column packet will fit in the buffer. */
  if (con->buffer_size < con->packet_size)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (con->packet_size == 5 && con->buffer_ptr[0] == 254)
  {
    /* EOF packet marking end of columns. */
    con->result->column= NULL;
    con->result->warning_count= drizzle_get_byte2(con->buffer_ptr + 1);
    con->status= drizzle_status_t(drizzle_get_byte2(con->buffer_ptr + 3));
    con->buffer_ptr+= 5;
    con->buffer_size-= 5;

    con->pop_state();
  }
  else if (con->result->options & DRIZZLE_RESULT_SKIP_COLUMN)
  {
    con->buffer_ptr+= con->packet_size;
    con->buffer_size-= con->packet_size;
    con->packet_size= 0;
    con->result->column_current++;

    con->pop_state();
  }
  else
  {
    if (con->result->column == NULL)
    {
      column= drizzle_column_create(con->result);
      if (column == NULL)
        return DRIZZLE_RETURN_MEMORY;

      con->result->column= column;
    }
    else
    {
      column= con->result->column;
    }

    /* These functions can only fail if they need to read data, but we know we
       buffered the entire packet, so ignore returns. */
    (void)drizzle_unpack_string(con, column->catalog, DRIZZLE_MAX_CATALOG_SIZE);
    (void)drizzle_unpack_string(con, column->db, DRIZZLE_MAX_DB_SIZE);
    (void)drizzle_unpack_string(con, column->table, DRIZZLE_MAX_TABLE_SIZE);
    (void)drizzle_unpack_string(con, column->orig_table,
                                DRIZZLE_MAX_TABLE_SIZE);
    (void)drizzle_unpack_string(con, column->name,
                                DRIZZLE_MAX_COLUMN_NAME_SIZE);
    (void)drizzle_unpack_string(con, column->orig_name,
                                DRIZZLE_MAX_COLUMN_NAME_SIZE);

    /* Skip one filler byte. */
    column->charset= (drizzle_charset_t)drizzle_get_byte2(con->buffer_ptr + 1);
    column->max_size= drizzle_get_byte4(con->buffer_ptr + 3);

    column->type= drizzle_column_type_t(con->buffer_ptr[7]);

    column->flags= drizzle_get_byte2(con->buffer_ptr + 8);
    if (column->type <= DRIZZLE_COLUMN_TYPE_INT24 &&
        column->type != DRIZZLE_COLUMN_TYPE_TIMESTAMP)
    {
      column->flags|= DRIZZLE_COLUMN_FLAGS_NUM;
    }

    column->decimals= con->buffer_ptr[10];
    /* Skip two reserved bytes. */

    con->buffer_ptr+= 13;
    con->buffer_size-= 13;
    con->packet_size-= 13;

    if (con->packet_size > 0)
    {
      drizzle_column_set_default_value(column, con->buffer_ptr,
                                       con->packet_size);

      con->buffer_ptr+= con->packet_size;
      con->buffer_size-= con->packet_size;
    }
    else
      column->default_value[0]= 0;

    con->result->column_current++;

    con->pop_state();
  }

  return DRIZZLE_RETURN_OK;
}

