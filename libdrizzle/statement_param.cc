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
#include "libdrizzle/common.h"

/* Internal function */
drizzle_return_t drizzle_stmt_set_param(drizzle_stmt_st *stmt, uint16_t param_num, drizzle_column_type_t type, void *data, uint32_t length, bool is_unsigned)
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
  stmt->query_params[param_num].options.is_unsigned= is_unsigned;
  stmt->query_params[param_num].is_bound= true;

  return DRIZZLE_RETURN_OK;
}


drizzle_return_t drizzle_stmt_set_tiny(drizzle_stmt_st *stmt, uint16_t param_num, uint8_t value, bool is_unsigned)
{
  uint8_t *val;
  val= (uint8_t*) stmt->query_params[param_num].data_buffer;
  *val= value;

  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_TINY, val, 1, is_unsigned);
}
drizzle_return_t drizzle_stmt_set_short(drizzle_stmt_st *stmt, uint16_t param_num, uint16_t value, bool is_unsigned)
{
  uint16_t *val;
  val= (uint16_t*) stmt->query_params[param_num].data_buffer;
  *val= value;

  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_SHORT, val, 2, is_unsigned);
}

drizzle_return_t drizzle_stmt_set_int(drizzle_stmt_st *stmt, uint16_t param_num, uint32_t value, bool is_unsigned)
{
  uint32_t *val;
  val= (uint32_t*) stmt->query_params[param_num].data_buffer;
  *val= value;

  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_LONG, val, 4, is_unsigned);
}

drizzle_return_t drizzle_stmt_set_bigint(drizzle_stmt_st *stmt, uint16_t param_num, uint64_t value, bool is_unsigned)
{
  uint64_t *val;
  val= (uint64_t*) stmt->query_params[param_num].data_buffer;
  *val= value;

  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_LONGLONG, val, 8, is_unsigned);
}

drizzle_return_t drizzle_stmt_set_double(drizzle_stmt_st *stmt, uint16_t param_num, double value)
{
  double *val;
  val= (double*) stmt->query_params[param_num].data_buffer;
  *val= value;

  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_DOUBLE, val, 8, false);
}

drizzle_return_t drizzle_stmt_set_float(drizzle_stmt_st *stmt, uint16_t param_num, float value)
{
  float *val;
  val= (float*) stmt->query_params[param_num].data_buffer;
  *val= value;

  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_FLOAT, val, 4, false);
}

drizzle_return_t drizzle_stmt_set_string(drizzle_stmt_st *stmt, uint16_t param_num, char *value, size_t length)
{
  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_STRING, value, length, false);
}

drizzle_return_t drizzle_stmt_set_null(drizzle_stmt_st *stmt, uint16_t param_num)
{
  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_NULL, NULL, 0, false);
}

drizzle_return_t drizzle_stmt_set_time(drizzle_stmt_st *stmt, uint16_t param_num, uint32_t days, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t microseconds, bool is_negative)
{
  drizzle_datetime_st *time;
  time= (drizzle_datetime_st*) stmt->query_params[param_num].data_buffer;

  time->negative= is_negative;
  time->day= days;
  time->hour= hours;
  time->minute= minutes;
  time->second= seconds;
  time->microsecond= microseconds;

  /* Length not important because we will figure that out when packing */
  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_TIME, time, 0, false);
}

drizzle_return_t drizzle_stmt_set_timestamp(drizzle_stmt_st *stmt, uint16_t param_num, uint16_t year, uint8_t month, uint8_t day, uint8_t hours, uint8_t minutes, uint8_t seconds, uint32_t microseconds)
{
  drizzle_datetime_st *timestamp;
  timestamp= (drizzle_datetime_st*) stmt->query_params[param_num].data_buffer;

  timestamp->negative= false;
  timestamp->year= year;
  timestamp->day= day;
  timestamp->month= month;
  timestamp->year= year;
  timestamp->hour= hours;
  timestamp->minute= minutes;
  timestamp->second= seconds;
  timestamp->microsecond= microseconds;

  /* Length not important because we will figure that out when packing */ 
  return drizzle_stmt_set_param(stmt, param_num, DRIZZLE_COLUMN_TYPE_TIME, timestamp, 0, false);
}

bool drizzle_stmt_get_is_null_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
{
  uint16_t column_number;
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  column_number=  drizzle_stmt_column_lookup(stmt->prepare_result, column_name, ret_ptr);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return 0;
  }
  return drizzle_stmt_get_is_null(stmt, column_number, ret_ptr);
}

bool drizzle_stmt_get_is_null(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
{
  if ((stmt == NULL) || (stmt->result_params == NULL) || (column_number >= stmt->execute_result->column_count))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return false;
  }

  *ret_ptr= DRIZZLE_RETURN_OK;
  return stmt->result_params[column_number].options.is_null;
}

bool drizzle_stmt_get_is_unsigned_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
{
  uint16_t column_number;
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  column_number=  drizzle_stmt_column_lookup(stmt->prepare_result, column_name, ret_ptr);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return 0;
  }
  return drizzle_stmt_get_is_unsigned(stmt, column_number, ret_ptr);
}

bool drizzle_stmt_get_is_unsigned(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
{
  if ((stmt == NULL) || (stmt->result_params == NULL) || (column_number >= stmt->execute_result->column_count))
  { 
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return false;
  }

  *ret_ptr= DRIZZLE_RETURN_OK;
  return stmt->result_params[column_number].options.is_unsigned;
}

const char *drizzle_stmt_get_string_from_name(drizzle_stmt_st *stmt, const char *column_name, size_t *len, drizzle_return_t *ret_ptr)
{
  uint16_t column_number;
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  column_number=  drizzle_stmt_column_lookup(stmt->prepare_result, column_name, ret_ptr);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return 0;
  }
  return drizzle_stmt_get_string(stmt, column_number, len, ret_ptr);
}

const char *drizzle_stmt_get_string(drizzle_stmt_st *stmt, uint16_t column_number, size_t *len, drizzle_return_t *ret_ptr)
{
  char *val;
  drizzle_bind_st *param;
  if ((stmt == NULL) || (stmt->result_params == NULL) || (column_number >= stmt->execute_result->column_count))
  { 
    *len= 0;
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return NULL;
  }

  param= &stmt->result_params[column_number];
  *ret_ptr= DRIZZLE_RETURN_OK;
  switch(param->type)
  {
    case DRIZZLE_COLUMN_TYPE_NULL:
      *ret_ptr= DRIZZLE_RETURN_NULL_SIZE;
      val= 0;
      break;
    case DRIZZLE_COLUMN_TYPE_TINY:
      val= long_to_string(param, (uint32_t)(*(uint8_t*)param->data));
      *len= strlen(val);
      break;
    case DRIZZLE_COLUMN_TYPE_SHORT:
    case DRIZZLE_COLUMN_TYPE_YEAR:
      val= long_to_string(param, (uint32_t)(*(uint16_t*)param->data));
      *len= strlen(val);
      break;
    case DRIZZLE_COLUMN_TYPE_INT24:
    case DRIZZLE_COLUMN_TYPE_LONG:
      val= long_to_string(param, *(uint32_t*)param->data);
      *len= strlen(val);
      break;
    case DRIZZLE_COLUMN_TYPE_LONGLONG:
      val= longlong_to_string(param, *(uint64_t*)param->data);
      *len= strlen(val);
      break;
    case DRIZZLE_COLUMN_TYPE_FLOAT:
      val= double_to_string(param, (double) (*(float*)param->data));
      *len= strlen(val);
      break;
    case DRIZZLE_COLUMN_TYPE_DOUBLE:
      val= double_to_string(param, *(double*)param->data);
      *len= strlen(val);
      break;
    case DRIZZLE_COLUMN_TYPE_TIME:
      val= time_to_string(param, (drizzle_datetime_st*)param->data);
      *len= strlen(val);
      break;
    case DRIZZLE_COLUMN_TYPE_DATE:
    case DRIZZLE_COLUMN_TYPE_DATETIME:
    case DRIZZLE_COLUMN_TYPE_TIMESTAMP:
      val= timestamp_to_string(param, (drizzle_datetime_st*)param->data);
      *len= strlen(val);
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
      val= (char*)param->data;
      *len= param->length;
      break;
    case DRIZZLE_COLUMN_TYPE_NEWDATE:
    case DRIZZLE_COLUMN_TYPE_VARCHAR:
    case DRIZZLE_COLUMN_TYPE_ENUM:
    case DRIZZLE_COLUMN_TYPE_SET:
    case DRIZZLE_COLUMN_TYPE_GEOMETRY:
    case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
    case DRIZZLE_COLUMN_TYPE_DATETIME2:
    case DRIZZLE_COLUMN_TYPE_TIME2:
    default:
      *ret_ptr= DRIZZLE_RETURN_INVALID_CONVERSION;
      val= NULL;
      *len= 0;
  }

  return val;
}

uint32_t drizzle_stmt_get_int_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
{
  uint16_t column_number;
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  column_number=  drizzle_stmt_column_lookup(stmt->prepare_result, column_name, ret_ptr);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return 0;
  }
  return drizzle_stmt_get_int(stmt, column_number, ret_ptr);
}

uint32_t drizzle_stmt_get_int(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
{
  uint32_t val;
  drizzle_bind_st *param;

  if ((stmt == NULL) || (stmt->result_params == NULL) || (column_number >= stmt->execute_result->column_count))
  { 
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return 0;
  }

  param= &stmt->result_params[column_number];
  *ret_ptr= DRIZZLE_RETURN_OK;
  switch(param->type)
  {
    case DRIZZLE_COLUMN_TYPE_NULL:
      *ret_ptr= DRIZZLE_RETURN_NULL_SIZE;
      val= 0;
      break;
    case DRIZZLE_COLUMN_TYPE_TINY:
      val= (uint32_t) (*(uint8_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_SHORT:
    case DRIZZLE_COLUMN_TYPE_YEAR:
      val= (uint32_t) (*(uint16_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_INT24:
    case DRIZZLE_COLUMN_TYPE_LONG:
      val= (uint32_t) (*(uint32_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_LONGLONG:
      val= (uint32_t) (*(uint64_t*)param->data);
      if (val > UINT32_MAX)
      {
        *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      }
      break;
    case DRIZZLE_COLUMN_TYPE_FLOAT:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (uint32_t) (*(float*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_DOUBLE:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (uint32_t) (*(double*)param->data);
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
    case DRIZZLE_COLUMN_TYPE_VARCHAR:
    case DRIZZLE_COLUMN_TYPE_ENUM:
    case DRIZZLE_COLUMN_TYPE_SET:
    case DRIZZLE_COLUMN_TYPE_GEOMETRY:
    case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
    case DRIZZLE_COLUMN_TYPE_DATETIME2:
    case DRIZZLE_COLUMN_TYPE_TIME2:
    default:
      *ret_ptr= DRIZZLE_RETURN_INVALID_CONVERSION;
      val= 0;
  }

  return val;
}

uint64_t drizzle_stmt_get_bigint_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
{
  uint16_t column_number;
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  column_number=  drizzle_stmt_column_lookup(stmt->prepare_result, column_name, ret_ptr);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return 0;
  }
  return drizzle_stmt_get_bigint(stmt, column_number, ret_ptr);
}

uint64_t drizzle_stmt_get_bigint(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
{
  uint32_t val;
  drizzle_bind_st *param;

  if ((stmt == NULL) || (stmt->result_params == NULL) || (column_number >= stmt->execute_result->column_count))
  { 
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return 0;
  }

  param= &stmt->result_params[column_number];
  *ret_ptr= DRIZZLE_RETURN_OK;
  switch(param->type)
  {
    case DRIZZLE_COLUMN_TYPE_NULL:
      *ret_ptr= DRIZZLE_RETURN_NULL_SIZE;
      val= 0;
      break;
    case DRIZZLE_COLUMN_TYPE_TINY:
      val= (uint64_t) (*(uint8_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_SHORT:
    case DRIZZLE_COLUMN_TYPE_YEAR:
      val= (uint64_t) (*(uint16_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_INT24:
    case DRIZZLE_COLUMN_TYPE_LONG:
      val= (uint64_t) (*(uint32_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_LONGLONG:
      val= (uint64_t) (*(uint64_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_FLOAT:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (uint64_t) (*(float*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_DOUBLE:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (uint64_t) (*(double*)param->data);
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
    case DRIZZLE_COLUMN_TYPE_VARCHAR:
    case DRIZZLE_COLUMN_TYPE_ENUM:
    case DRIZZLE_COLUMN_TYPE_SET:
    case DRIZZLE_COLUMN_TYPE_GEOMETRY:
    case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
    case DRIZZLE_COLUMN_TYPE_DATETIME2:
    case DRIZZLE_COLUMN_TYPE_TIME2:
    default:
      *ret_ptr= DRIZZLE_RETURN_INVALID_CONVERSION;
      val= 0;
  }

  return val;
}

double drizzle_stmt_get_double_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
{
  uint16_t column_number;
  if ((stmt == NULL) || (stmt->result_params == NULL))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
  }
  column_number=  drizzle_stmt_column_lookup(stmt->prepare_result, column_name, ret_ptr);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return 0;
  }
  return drizzle_stmt_get_double(stmt, column_number, ret_ptr);
}

double drizzle_stmt_get_double(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
{
  double val;
  drizzle_bind_st *param;
  if ((stmt == NULL) || (stmt->result_params == NULL) || (column_number >= stmt->execute_result->column_count))
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return 0;
  }
  param= &stmt->result_params[column_number];
  *ret_ptr= DRIZZLE_RETURN_OK;
  switch(param->type)
  {
    case DRIZZLE_COLUMN_TYPE_NULL:
      *ret_ptr= DRIZZLE_RETURN_NULL_SIZE;
      val= 0;
      break;
    case DRIZZLE_COLUMN_TYPE_TINY:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (double) (*(uint8_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_SHORT:
    case DRIZZLE_COLUMN_TYPE_YEAR:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (double) (*(uint16_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_INT24:
    case DRIZZLE_COLUMN_TYPE_LONG:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (double) (*(uint32_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_LONGLONG:
      *ret_ptr= DRIZZLE_RETURN_TRUNCATED;
      val= (double) (*(uint64_t*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_FLOAT:
      val= (double) (*(float*)param->data);
      break;
    case DRIZZLE_COLUMN_TYPE_DOUBLE:
      val= (uint32_t) (*(double*)param->data);
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
    case DRIZZLE_COLUMN_TYPE_VARCHAR:
    case DRIZZLE_COLUMN_TYPE_ENUM:
    case DRIZZLE_COLUMN_TYPE_SET:
    case DRIZZLE_COLUMN_TYPE_GEOMETRY:
    case DRIZZLE_COLUMN_TYPE_TIMESTAMP2:
    case DRIZZLE_COLUMN_TYPE_DATETIME2:
    case DRIZZLE_COLUMN_TYPE_TIME2:
    default:
      *ret_ptr= DRIZZLE_RETURN_INVALID_CONVERSION;
      val= 0;
  }

  return val;
}

char *long_to_string(drizzle_bind_st *param, uint32_t val)
{
  /* Pick an empty point in the buffer to make the str */
  char* buffer= param->data_buffer + 50;
  if (param->options.is_unsigned)
  {
    snprintf(buffer, 12, "%"PRIu32, val);
  }
  else
  {
    snprintf(buffer, 12, "%"PRId32, (int32_t)val);
  }
  return buffer;
}

char *longlong_to_string(drizzle_bind_st *param, uint64_t val)
{ 
  /* Max length is -INT64_MAX + NUL = 21 */
  char* buffer= param->data_buffer + 50;
  if (param->options.is_unsigned)
  { 
    snprintf(buffer, 21, "%"PRIu64, val);
  }
  else
  {
    snprintf(buffer, 21, "%"PRId64, (int64_t)val);
  }
  return buffer;
}

char *double_to_string(drizzle_bind_st *param, double val)
{ 
  /* Max length is 23 */
  char* buffer= param->data_buffer + 50;
  snprintf(buffer, 23, "%f", val);
  return buffer;
}

char *time_to_string(drizzle_bind_st *param, drizzle_datetime_st *time)
{
  /* Max time is -HHH:MM:SS.ssssss + NUL = 17 */
  char* buffer= param->data_buffer + 50;
  if (time->microsecond == 0)
  {
    snprintf(buffer, 17, "%s%"PRIu16":%"PRIu8":%"PRIu8, (time->negative) ? "-" : "", time->hour, time->minute, time->second);
  }
  else
  {
    snprintf(buffer, 17, "%s%"PRIu16":%"PRIu8":%"PRIu8".%"PRIu32, (time->negative) ? "-" : "", time->hour, time->minute, time->second, time->microsecond);
  }
  return buffer;
}

char *timestamp_to_string(drizzle_bind_st *param, drizzle_datetime_st *timestamp)
{
  /* Max timestamp is YYYY-MM-DD HH:MM:SS.ssssss + NUL = 26 */
  char* buffer= param->data_buffer + 50;
  if (timestamp->microsecond == 0)
  {
    snprintf(buffer, 26, "%"PRIu16"-%"PRIu8"-%"PRIu32" %"PRIu16":%"PRIu8":%"PRIu8, timestamp->year, timestamp->month, timestamp->day, timestamp->hour, timestamp->minute, timestamp->second);
  }
  else
  {
    snprintf(buffer, 26, "%"PRIu16"-%"PRIu8"-%"PRIu32" %"PRIu16":%"PRIu8":%"PRIu8".%"PRIu32, timestamp->year, timestamp->month, timestamp->day, timestamp->hour, timestamp->minute, timestamp->second, timestamp->microsecond);
  }
  return buffer;
}

uint16_t drizzle_stmt_column_lookup(drizzle_result_st *result, const char *column_name, drizzle_return_t *ret_ptr)
{
  uint16_t current_column;
  for (current_column= 0; current_column < result->column_count; current_column++)
  {
    if (strncmp(column_name, result->column_buffer[current_column].name, DRIZZLE_MAX_COLUMN_NAME_SIZE) == 0)
    {
      *ret_ptr= DRIZZLE_RETURN_OK;
      return current_column;
    }
  }
  *ret_ptr= DRIZZLE_RETURN_NOT_FOUND;
  return 0;
}
