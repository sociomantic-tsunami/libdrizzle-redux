/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012 Andrew Hutchings (andrew@linuxjedi.co.uk)
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
#include "libdrizzle/common.h"

drizzle_stmt_st *drizzle_stmt_prepare(drizzle_st *con, const char *statement, size_t size, drizzle_return_t *ret_ptr)
{
  drizzle_stmt_st *stmt= (drizzle_stmt_st*)malloc(sizeof(drizzle_stmt_st));
  if (stmt == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_MEMORY;
    drizzle_set_error(con, __func__, "malloc");
    return NULL;
  }
  stmt->execute_result= NULL;
  stmt->param_count= 0;
  stmt->id= 0;
  stmt->query_params= NULL;
  stmt->result_params= NULL;
  stmt->null_bitmap_length= 0;
  stmt->null_bitmap= NULL;
  stmt->new_bind= true;
  con->stmt= stmt;
  stmt->con= con;

  stmt->prepare_result= drizzle_command_write(con, NULL, DRIZZLE_COMMAND_STMT_PREPARE,
                                      statement, size, size, ret_ptr);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    free(stmt);
    con->stmt= NULL;
    return NULL;
  }

  /* Don't get the unused parameter packets.  Format is the same as column
   * packets.  Deliberate off-by-one for the EOF packet */
  if (stmt->param_count)
  {
    uint16_t param_num;
    for (param_num= 0; param_num <= stmt->param_count; param_num++)
    {
      *ret_ptr= drizzle_column_skip(stmt->prepare_result);
      if ((*ret_ptr != DRIZZLE_RETURN_OK) && (*ret_ptr != DRIZZLE_RETURN_EOF))
      {
        free(stmt);
        return NULL;
      }
    }
  }
  
  /* Reset column counter which is incremented when skipping parameter packets
   * and then buffer the columns */

  stmt->prepare_result->column_current= 0;
  drizzle_column_buffer(stmt->prepare_result);

  /* Parameter count can then be used to figure out the length of the null
   * bitmap mask */

  stmt->null_bitmap_length= (stmt->param_count + 7) / 8;
  stmt->null_bitmap= (uint8_t*)calloc(stmt->null_bitmap_length, 1);
  if (stmt->null_bitmap == NULL)
  {
    free(stmt);
    *ret_ptr= DRIZZLE_RETURN_MEMORY;
    drizzle_set_error(con, __func__, "malloc");
    return NULL;
  }

  /* Also use the parameter count to allocate the parameters */
  stmt->query_params= (drizzle_bind_st*)calloc(stmt->param_count, sizeof(drizzle_bind_st));
  stmt->state= DRIZZLE_STMT_PREPARED;
  stmt->fields= stmt->prepare_result->column_buffer;

  return stmt;
}

drizzle_return_t drizzle_stmt_bind_param(drizzle_stmt_st *stmt, uint16_t param_num, drizzle_column_type_t type, void *data, uint32_t length, drizzle_bind_options_t options)
{
  if ((stmt == NULL) || (param_num >= stmt->param_count) || (data == NULL))
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  if (stmt->state < DRIZZLE_STMT_PREPARED)
  {
    drizzle_set_error(stmt->con, __func__, "stmt object has bot been prepared");
    return DRIZZLE_RETURN_STMT_ERROR;
  }

  stmt->query_params[param_num].type= type;
  stmt->query_params[param_num].data= data;
  stmt->query_params[param_num].length= length;
  stmt->query_params[param_num].options= options;
  stmt->query_params[param_num].is_bound= true;

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_stmt_execute(drizzle_stmt_st *stmt)
{
  uint16_t current_param;
  drizzle_bind_st *param_ptr;
  size_t param_lengths= 0;
  size_t buffer_size= 0;
  uint8_t *buffer;
  uint8_t *buffer_pos;
  uint8_t *data_pos;
  drizzle_return_t ret;

  /* Calculate param lengths */
  for (current_param= 0; current_param < stmt->param_count; current_param++)
  {
    if (!stmt->query_params[current_param].is_bound)
    {
      drizzle_set_error(stmt->con, __func__, "parameter %d has not been bound", current_param);
      return DRIZZLE_RETURN_STMT_ERROR;
    }
    /* parameter length + 8 byte header */
    param_lengths+= stmt->query_params[current_param].length + 8;
  }

  buffer_size= 4 /* Statement ID */
             + 1 /* Flags */
             + 4 /* Reserved (always set to 1) */
             + stmt->null_bitmap_length /* Null bitmap length */
             + 1 /* New parameters bound flag */
             + (stmt->param_count * 2) /* Parameter type data */
             + param_lengths; /* Parameter data */

  buffer = (uint8_t*)malloc(buffer_size);
  if (buffer == NULL)
  {
    drizzle_set_error(stmt->con, __func__, "malloc");
    return DRIZZLE_RETURN_MEMORY;
  }
  buffer_pos= buffer;

  /* Statement ID */
  drizzle_set_byte4(buffer, stmt->id);
  /* Flags (not currently used) */
  buffer[4]= 0;
  /* Reserved, protocol specifies set to 1 */
  drizzle_set_byte4(&buffer[5], 1);
  buffer_pos+= 9;
  /* Null bitmap */
  memcpy(buffer_pos, stmt->null_bitmap, stmt->null_bitmap_length);
  buffer_pos+= stmt->null_bitmap_length;
  /* New parameters bound flag
   * If set to 1 then we need to leave a gap between the parameters type data
   * and the actually data, so we keep track of a second pointer in the
   * buffer for this
   * */
  if (stmt->new_bind)
  {
    *buffer_pos= 1;
    buffer_pos++;
    /* Each param has a 2 byte data type header so the data pointer should be
     * moved to this */
    data_pos= buffer_pos + (stmt->param_count * 2);
  }
  else
  {
    *buffer_pos= 0;
    buffer_pos++;
    data_pos= buffer_pos;
  }
  
  /* Go through each param copying to buffer 
   * In an ideal world we would do this without the copy
   * */
  for (current_param= 0; current_param < stmt->param_count; current_param++)
  {
    uint16_t short_value;
    uint32_t long_value;
    uint64_t longlong_value;
    param_ptr= &stmt->query_params[current_param];

    if (stmt->new_bind)
    {
      uint16_t type= (uint16_t)param_ptr->type;
      if (param_ptr->options & DRIZZLE_BIND_OPTION_UNSIGNED)
      {
        /* Set the unsigned bit flag on the type data */
        type |= 0x8000;
      }
      drizzle_set_byte2(buffer_pos, type);
      buffer_pos+= 2;
    }

    if (param_ptr->options & DRIZZLE_BIND_OPTION_LONG_DATA)
    {
      /* Long data is sent separately, not in this buffer */
      continue;
    }

    switch(param_ptr->type)
    {
      case DRIZZLE_COLUMN_TYPE_NULL:
        /* Toggle the bit for this column in the bitmap */
        *stmt->null_bitmap |= (current_param ^ 2);
        break;
      case DRIZZLE_COLUMN_TYPE_TINY:
        *data_pos= *(uint8_t*)param_ptr->data;
        data_pos++;
        break;
      case DRIZZLE_COLUMN_TYPE_SHORT:
        short_value= *(uint16_t*)param_ptr->data;
        drizzle_set_byte2(data_pos, short_value);
        data_pos+= 2;
        break;
      case DRIZZLE_COLUMN_TYPE_LONG:
        long_value= *(uint32_t*)param_ptr->data;
        drizzle_set_byte4(data_pos, long_value);
        data_pos+= 4;
        break;
      case DRIZZLE_COLUMN_TYPE_LONGLONG:
        longlong_value= *(uint64_t*)param_ptr->data;
        drizzle_set_byte8(data_pos, longlong_value);
        data_pos+= 8;
        break;
      case DRIZZLE_COLUMN_TYPE_FLOAT:
        /* Float and double don't need to be packed apparently */
        memcpy(data_pos, param_ptr->data, 4);
        data_pos+= 4;
        break;
      case DRIZZLE_COLUMN_TYPE_DOUBLE:
        memcpy(data_pos, param_ptr->data, 8);
        data_pos+= 8;
        break;
      case DRIZZLE_COLUMN_TYPE_TIME:
        data_pos= drizzle_pack_time((drizzle_datetime_st*)param_ptr->data, data_pos);
        break;
      case DRIZZLE_COLUMN_TYPE_DATE:
      case DRIZZLE_COLUMN_TYPE_DATETIME:
      case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
        data_pos= drizzle_pack_datetime((drizzle_datetime_st*)param_ptr->data, data_pos);
        break;
      case DRIZZLE_COLUMN_TYPE_TINY_BLOB:
      case DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB:
      case DRIZZLE_COLUMN_TYPE_LONG_BLOB:
      case DRIZZLE_COLUMN_TYPE_BLOB:
      case DRIZZLE_COLUMN_TYPE_VARCHAR:
      case DRIZZLE_COLUMN_TYPE_VAR_STRING:
      case DRIZZLE_COLUMN_TYPE_STRING:
      case DRIZZLE_COLUMN_TYPE_DECIMAL:
      case DRIZZLE_COLUMN_TYPE_NEWDECIMAL:
        data_pos= drizzle_pack_binary((uint8_t*)param_ptr->data, param_ptr->length, data_pos);
        break;
      /* These types aren't handled yet, most are for older MySQL versions */
      case DRIZZLE_COLUMN_TYPE_INT24:
      case DRIZZLE_COLUMN_TYPE_YEAR:
      case DRIZZLE_COLUMN_TYPE_NEWDATE:
      case DRIZZLE_COLUMN_TYPE_ENUM:
      case DRIZZLE_COLUMN_TYPE_SET:
      case DRIZZLE_COLUMN_TYPE_GEOMETRY:
      case DRIZZLE_COLUMN_TYPE_BIT:
      default:
        drizzle_set_error(stmt->con, __func__, "unknown type when filling buffer");
        free(buffer);
        return DRIZZLE_RETURN_UNEXPECTED_DATA;
        break;
    }
  }
  if (stmt->execute_result)
  {
    drizzle_result_free(stmt->execute_result);
    stmt->execute_result= NULL;
  }

  /* Set buffer size to what we actually used */
  buffer_size= data_pos - buffer;

  stmt->execute_result= drizzle_command_write(stmt->con, stmt->execute_result, DRIZZLE_COMMAND_STMT_EXECUTE, buffer, buffer_size, buffer_size, &ret);

  if (ret == DRIZZLE_RETURN_OK)
  {
    stmt->state= DRIZZLE_STMT_EXECUTED;
  }
  else
  {
    free(buffer);
    return ret;
  }

  stmt->new_bind= false;
  
  stmt->execute_result->binary_rows= true;

  stmt->execute_result->options= (drizzle_result_options_t)((uint8_t)stmt->execute_result->options | (uint8_t)DRIZZLE_RESULT_BINARY_ROWS);

  if (stmt->execute_result->column_count > 0)
  {
    ret= drizzle_column_buffer(stmt->execute_result);
    stmt->result_params= (drizzle_bind_st*)calloc(stmt->execute_result->column_count, sizeof(drizzle_bind_st));
  }

  free(buffer);
  return ret;
}

drizzle_return_t drizzle_stmt_send_long_data(drizzle_stmt_st *stmt, uint16_t param_num, uint8_t *data, size_t len)
{
  drizzle_return_t ret;
  uint8_t *buffer;

  if ((stmt == NULL) || (param_num >= stmt->param_count))
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (stmt->state < DRIZZLE_STMT_PREPARED)
  {
    drizzle_set_error(stmt->con, __func__, "stmt object has bot been prepared");
    return DRIZZLE_RETURN_STMT_ERROR;
  }


  /* TODO: rework drizzle_command_write so we can send a header and we don't
   * need this copy
   * */
  buffer= (uint8_t*)malloc(len + 6);

  drizzle_set_byte4(buffer, stmt->id);
  drizzle_set_byte2(&buffer[4], param_num);
  memcpy(&buffer[6], data, len);

  stmt->con->options= (drizzle_options_t)((uint8_t)stmt->con->options | (uint8_t)DRIZZLE_CON_NO_RESULT_READ);
  drizzle_command_write(stmt->con, NULL, DRIZZLE_COMMAND_STMT_SEND_LONG_DATA,
                        buffer, len+6, len+6, &ret);
  stmt->con->options= (drizzle_options_t)((uint8_t)stmt->con->options & (uint8_t)~DRIZZLE_CON_NO_RESULT_READ);
  stmt->query_params[param_num].options= (drizzle_bind_options_t)((uint8_t)stmt->query_params[param_num].options | (uint8_t)DRIZZLE_BIND_OPTION_LONG_DATA);

  free(buffer);
  return ret;
}

drizzle_return_t drizzle_stmt_reset(drizzle_stmt_st *stmt)
{
  drizzle_return_t ret;
  uint8_t buffer[4];
  uint16_t current_param;

  if (stmt == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  for (current_param= 0; current_param < stmt->param_count; current_param++)
  {
    stmt->query_params[current_param].options= (drizzle_bind_options_t)((uint8_t)stmt->query_params[current_param].options & (uint8_t)~DRIZZLE_BIND_OPTION_LONG_DATA);
  }

  drizzle_set_byte4(buffer, stmt->id);
  stmt->con->options= (drizzle_options_t)((uint8_t)stmt->con->options | (uint8_t)DRIZZLE_CON_NO_RESULT_READ);
  drizzle_command_write(stmt->con, NULL, DRIZZLE_COMMAND_STMT_RESET, buffer, 4,
                        4, &ret);
  stmt->con->options= (drizzle_options_t)((uint8_t)stmt->con->options & (uint8_t)~DRIZZLE_CON_NO_RESULT_READ);
  if (stmt->execute_result)
  {
    drizzle_result_free(stmt->execute_result);
    stmt->execute_result= NULL;
  }
  stmt->state= DRIZZLE_STMT_PREPARED;
  free(stmt->result_params);

  return ret;
}

drizzle_return_t drizzle_stmt_fetch(drizzle_stmt_st *stmt)
{
  drizzle_return_t ret= DRIZZLE_RETURN_OK;
  uint16_t column_counter= 0;
  drizzle_row_t row;
  drizzle_column_st *column;

  if (stmt == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (stmt->state < DRIZZLE_STMT_EXECUTED)
  {
    drizzle_set_error(stmt->con, __func__, "statement has not been executed");
    return DRIZZLE_RETURN_UNEXPECTED_DATA;
  }
  stmt->con->result= stmt->execute_result;

  /* Determine how to read the row based on whether or not it is already
   * buffered */

  if (stmt->execute_result->options & DRIZZLE_RESULT_BUFFER_ROW)
  {
    row= drizzle_row_next(stmt->execute_result);
  }
  else
  {
    row= drizzle_row_buffer(stmt->execute_result, &ret);
  }

  if (row == NULL)
  {
    return DRIZZLE_RETURN_ROW_END;
  }
  drizzle_column_seek(stmt->execute_result, 0);

  while((column= drizzle_column_next(stmt->execute_result)))
  {
    drizzle_bind_st *param= &stmt->result_params[column_counter];
    /* if this row is null in the result bitmap */
    if (*stmt->execute_result->null_bitmap & ((column_counter^2) << 2))
    {
      param->options = (drizzle_bind_options_t)((uint8_t)param->options | (uint8_t) DRIZZLE_BIND_OPTION_NULL);
      param->length= 0;
    }
    else
    {
      uint16_t short_data;
      uint32_t long_data;
      uint64_t longlong_data;
      param->length= stmt->execute_result->field_sizes[column_counter];
      if (column->flags & DRIZZLE_COLUMN_FLAGS_UNSIGNED)
      {
        param->options = (drizzle_bind_options_t)((uint8_t)param->options | (uint8_t) DRIZZLE_BIND_OPTION_UNSIGNED);
      }

      param->data= realloc(param->data, param->length);
      param->type= column->type;

      switch(column->type)
      {
        case DRIZZLE_COLUMN_TYPE_NULL:
          /* I don't think this one should happen, but cover it anyway */
          param->length= 0;
          break;
        case DRIZZLE_COLUMN_TYPE_TINY:
          *(uint8_t*)param->data= *row[column_counter];
          break;
        case DRIZZLE_COLUMN_TYPE_SHORT:
        case DRIZZLE_COLUMN_TYPE_YEAR:
          short_data= drizzle_get_byte2(row[column_counter]);
          memcpy(param->data, &short_data, 2);
          break;
        case DRIZZLE_COLUMN_TYPE_INT24:
        case DRIZZLE_COLUMN_TYPE_LONG:
          long_data= drizzle_get_byte4(row[column_counter]);
          memcpy(param->data, &long_data, 4);
          break;
        case DRIZZLE_COLUMN_TYPE_LONGLONG:
          longlong_data= drizzle_get_byte8(row[column_counter]);
          memcpy(param->data, &longlong_data, 8);
          break;
        case DRIZZLE_COLUMN_TYPE_FLOAT:
          memcpy(param->data, row[column_counter], 4);
          break;
        case DRIZZLE_COLUMN_TYPE_DOUBLE:
          memcpy(param->data, row[column_counter], 8);
          break;
        case DRIZZLE_COLUMN_TYPE_TIME:
          drizzle_unpack_time(row[column_counter], param->length, (uint8_t*)param->data);
          break;
        case DRIZZLE_COLUMN_TYPE_DATE:
        case DRIZZLE_COLUMN_TYPE_DATETIME:
        case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
          drizzle_unpack_datetime(row[column_counter], param->length, (uint8_t*)param->data);
          break;
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
          memcpy(param->data, row[column_counter], param->length);
          break;
        /* These types aren't handled yet, most are for older MySQL versions */
        case DRIZZLE_COLUMN_TYPE_VARCHAR:
        case DRIZZLE_COLUMN_TYPE_ENUM:
        case DRIZZLE_COLUMN_TYPE_SET:
        case DRIZZLE_COLUMN_TYPE_GEOMETRY:
        default:
          drizzle_set_error(stmt->con, __func__, "Unknown data type found");
          ret= DRIZZLE_RETURN_UNEXPECTED_DATA;
          break;
      }
    }
    column_counter++;
    if (ret == DRIZZLE_RETURN_UNEXPECTED_DATA)
    {
      break;
    }
      
  }
  stmt->state= DRIZZLE_STMT_FETCHED;
  if (!(stmt->execute_result->options & DRIZZLE_RESULT_BUFFER_ROW))
  {
    drizzle_row_free(stmt->execute_result, row);
  }

  return ret;
}

drizzle_return_t drizzle_stmt_buffer(drizzle_stmt_st *stmt)
{
  if (stmt == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  if (stmt->state >= DRIZZLE_STMT_FETCHED)
  {
    drizzle_set_error(stmt->con, __func__, "data set has already been read");
    return DRIZZLE_RETURN_UNEXPECTED_DATA;
  }

  stmt->con->result= stmt->execute_result;
  stmt->state= DRIZZLE_STMT_FETCHED;

  return drizzle_result_buffer(stmt->execute_result);
}

drizzle_return_t drizzle_stmt_close(drizzle_stmt_st *stmt)
{
  uint8_t buffer[4];
  drizzle_return_t ret;

  if (stmt == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  free(stmt->null_bitmap);
  free(stmt->query_params);
  if (stmt->result_params)
  {
    free(stmt->result_params->data);
    free(stmt->result_params);
  }
  if (stmt->execute_result)
  {
    drizzle_result_free(stmt->execute_result);
  }
  if (stmt->prepare_result)
  {
    drizzle_result_free(stmt->prepare_result);
  }

  drizzle_set_byte4(buffer, stmt->id);
  stmt->con->options= (drizzle_options_t)((uint8_t)stmt->con->options | (uint8_t)DRIZZLE_CON_NO_RESULT_READ);
  drizzle_command_write(stmt->con, NULL, DRIZZLE_COMMAND_STMT_CLOSE, buffer, 4,
                        4, &ret);
  stmt->con->options= (drizzle_options_t)((uint8_t)stmt->con->options & (uint8_t)~DRIZZLE_CON_NO_RESULT_READ);
  free(stmt);
  return ret;
}

drizzle_column_type_t drizzle_stmt_item_type(drizzle_stmt_st *stmt, uint16_t column_number)
{
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    return DRIZZLE_COLUMN_TYPE_NONE;
  }
  return stmt->result_params[column_number].type;
}

void *drizzle_stmt_item_data(drizzle_stmt_st *stmt, uint16_t column_number)
{
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    return NULL;
  }
  return stmt->result_params[column_number].data;
}

uint32_t drizzle_stmt_item_length(drizzle_stmt_st *stmt, uint16_t column_number)
{
  if ((stmt == NULL) || (stmt->result_params == NULL))
  { 
    return 0;
  }
  return stmt->result_params[column_number].length;
}

drizzle_bind_options_t drizzle_stmt_item_options(drizzle_stmt_st *stmt, uint16_t column_number)
{
  if ((stmt == NULL) || (stmt->result_params == NULL))
  { 
    return DRIZZLE_BIND_OPTION_NONE;
  }
  return stmt->result_params[column_number].options;
}

uint16_t drizzle_stmt_column_count(drizzle_stmt_st *stmt)
{
  if ((stmt == NULL) || (stmt->prepare_result == NULL))
  {
    return 0;
  }

  return stmt->prepare_result->column_count;
}

uint64_t drizzle_stmt_affected_rows(drizzle_stmt_st *stmt)
{
  if ((stmt == NULL) || (stmt->execute_result == NULL))
  {
    return 0;
  }
  
  return stmt->execute_result->affected_rows;
}

uint64_t drizzle_stmt_insert_id(drizzle_stmt_st *stmt)
{
  if ((stmt == NULL) || (stmt->execute_result == NULL))
  {
    return 0;
  }

  return stmt->execute_result->insert_id;
}

uint16_t drizzle_stmt_param_count(drizzle_stmt_st *stmt)
{
  if (stmt == NULL)
  {
    return 0;
  }

  return stmt->param_count;
}

uint64_t drizzle_stmt_row_count(drizzle_stmt_st *stmt)
{
  if ((stmt == NULL) || (stmt->execute_result == NULL))
  {
    return 0;
  }

  return stmt->execute_result->row_count;
}
