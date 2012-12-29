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
#include <zlib.h>

drizzle_result_st *drizzle_start_binlog(drizzle_st *con,
                                            uint32_t server_id,
                                            const char *file,
                                            uint32_t start_position,
                                            drizzle_return_t *ret_ptr)
{ 
  uint8_t data[128];
  uint8_t *ptr;
  uint8_t len= 0, fn_len= 0;
  drizzle_result_st *result;

  // Hack in 5.6 to say that client support checksums
  result= drizzle_query_str(con, "SET @master_binlog_checksum='NONE'", ret_ptr);
  drizzle_result_free(result);
  if (*ret_ptr != DRIZZLE_RETURN_OK)
  {
    return NULL;
  }

  ptr= data;

  // Start position less than binlog magic size is wrong
  if (start_position < 4)
    start_position = 4;

  // Start position
  drizzle_set_byte4(ptr, start_position);
  ptr+= 4;
  // Binlog flags
  drizzle_set_byte2(ptr, 0);
  ptr+= 2;
  // Server ID
  drizzle_set_byte4(ptr, server_id);
  ptr+= 4;

  len= 4 +  // Start position
       2 +  // Binlog flags
       4;   // Server ID

  // Prevent buffer overflow with long binlog filenames
  if (file)
  {
    if (strlen(file) >= (size_t)(128 - len))
    {
      fn_len= 128 - len;
    }
    else
    {
      fn_len = strlen(file);
    }
    len+= fn_len;
    memcpy(ptr, file, fn_len);
  }

  return drizzle_command_write(con, NULL, DRIZZLE_COMMAND_BINLOG_DUMP,
                                   data, len, len, ret_ptr);
}

drizzle_return_t drizzle_binlog_get_next_event(drizzle_result_st *result)
{
  if (result == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_state_push(result->con, drizzle_state_binlog_read);
  drizzle_state_push(result->con, drizzle_state_packet_read);
  return drizzle_state_loop(result->con);
}

uint32_t drizzle_binlog_event_timestamp(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return 0;
  }

  return result->binlog_event->timestamp;
}

drizzle_binlog_event_types_t drizzle_binlog_event_type(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return drizzle_binlog_event_types_t();
  }

  return result->binlog_event->type;
}

uint32_t drizzle_binlog_event_server_id(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return 0;
  }

  return result->binlog_event->server_id;
}

uint32_t drizzle_binlog_event_length(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return 0;
  }

  return result->binlog_event->length;
}

uint32_t drizzle_binlog_event_next_pos(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return 0;
  }

  return result->binlog_event->next_pos;
}

uint16_t drizzle_binlog_event_flags(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return 0;
  }

  return result->binlog_event->flags;
}

uint16_t drizzle_binlog_event_extra_flags(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return 0;
  }

  return result->binlog_event->extra_flags;
}

const uint8_t *drizzle_binlog_event_data(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return NULL;
  }

  return result->binlog_event->data;
}

const uint8_t *drizzle_binlog_event_raw_data(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return NULL;
  }

  return result->binlog_event->raw_data;
}

uint32_t drizzle_binlog_event_raw_length(drizzle_result_st *result)
{
  if ((result == NULL) || (result->binlog_event == NULL))
  {
    return 0;
  }

  return result->binlog_event->raw_length;
}

drizzle_return_t drizzle_state_binlog_read(drizzle_st *con)
{
  drizzle_binlog_st *binlog_event;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (con->result->binlog_event == NULL)
  {
    con->result->binlog_event= (drizzle_binlog_st*)malloc(sizeof(drizzle_binlog_st));
    con->result->binlog_event->data= NULL;
  }
  binlog_event= con->result->binlog_event;

  if (con->packet_size != 0 && con->buffer_size < con->packet_size)
  {
    drizzle_state_push(con, drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (con->packet_size == 5 && con->buffer_ptr[0] == 254)
  {
    /* Got EOF packet, no more data. */
    con->result->warning_count= drizzle_get_byte2(con->buffer_ptr + 1);
    con->status= (drizzle_status_t)drizzle_get_byte2(con->buffer_ptr + 3);
    con->buffer_ptr+= 5;
    con->buffer_size-= 5;
    drizzle_state_pop(con);
    return DRIZZLE_RETURN_EOF;
  }
  else if (con->buffer_ptr[0] == 255)
  {
    con->result->error_code= drizzle_get_byte2(con->buffer_ptr + 1);
    con->error_code= con->result->error_code;
    /* Byte 3 is always a '#' character, skip it. */
    memcpy(con->result->sqlstate, con->buffer_ptr + 4,
           DRIZZLE_MAX_SQLSTATE_SIZE);
    con->result->sqlstate[DRIZZLE_MAX_SQLSTATE_SIZE]= 0;
    memcpy(con->sqlstate, con->result->sqlstate,
           DRIZZLE_MAX_SQLSTATE_SIZE + 1);
    con->buffer_ptr+= 9;
    con->buffer_size-= 9;
    con->packet_size-= 9;

    snprintf(con->last_error, DRIZZLE_MAX_ERROR_SIZE, "%.*s",
             (int32_t)con->packet_size, con->buffer_ptr);
    con->last_error[DRIZZLE_MAX_ERROR_SIZE-1]= 0;
    snprintf(con->result->info, DRIZZLE_MAX_INFO_SIZE, "%.*s",
             (int32_t)con->packet_size, con->buffer_ptr);
    con->result->info[DRIZZLE_MAX_INFO_SIZE-1]= 0;
    con->buffer_ptr+= con->packet_size;
    con->buffer_size-= con->packet_size;
    con->packet_size= 0;

    drizzle_state_pop(con);
    return DRIZZLE_RETURN_ERROR_CODE;
  }
  else
  {
    con->buffer_ptr++;
    con->packet_size--;
    con->buffer_size--;
    binlog_event->raw_data= con->buffer_ptr;
    binlog_event->timestamp= drizzle_get_byte4(con->buffer_ptr);
    binlog_event->type= (drizzle_binlog_event_types_t)con->buffer_ptr[4];
    binlog_event->server_id= drizzle_get_byte4(con->buffer_ptr + 5);
    binlog_event->raw_length= binlog_event->length= drizzle_get_byte4(con->buffer_ptr + 9);
    if (con->packet_size != binlog_event->length)
    {
        drizzle_set_error(con, "drizzle_state_binlog_read",
                          "packet size error:%zu:%zu", con->packet_size, binlog_event->length);
        return DRIZZLE_RETURN_UNEXPECTED_DATA;
    }
    if (binlog_event->length <= 27)
    {
      binlog_event->next_pos= drizzle_get_byte4(con->buffer_ptr + 13);
      binlog_event->flags= drizzle_get_byte2(con->buffer_ptr + 17);
      con->buffer_ptr+= binlog_event->length;
      con->buffer_size-= binlog_event->length;
      con->packet_size-= binlog_event->length;
      binlog_event->length= 0;
      free(binlog_event->data);
      binlog_event->data= NULL;
    }
    else
    {
      binlog_event->length= binlog_event->length -
                            19 - // Header length
                            8;   // Fixed rotate length
      binlog_event->next_pos= drizzle_get_byte4(con->buffer_ptr + 13);
      binlog_event->flags= drizzle_get_byte2(con->buffer_ptr + 17);
    
      con->buffer_ptr+= 27;
      con->buffer_size-= 27;
      con->packet_size-= 27;
      binlog_event->data= (uint8_t*)realloc(binlog_event->data, binlog_event->length);
      /* 5.6.1 or higher is automatic checksums on */
      if (binlog_event->type == DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION)
      {
        if (strncmp((const char*)con->buffer_ptr + 2, DRIZZLE_BINLOG_CHECKSUM_VERSION, strlen(DRIZZLE_BINLOG_CHECKSUM_VERSION)) <= 0)
        {
          con->result->binlog_checksums= true;
        }
      }
      /* A checksum is basically a CRC32 at the end of the event data (4 bytes) */
      if (con->result->binlog_checksums)
      {
        memcpy(binlog_event->data, con->buffer_ptr, binlog_event->length - DRIZZLE_BINLOG_CRC32_LEN);
      }
      else
      {
        memcpy(binlog_event->data, con->buffer_ptr, binlog_event->length);
      }
      con->buffer_ptr+= binlog_event->length;
      con->buffer_size-= binlog_event->length;
      con->packet_size-= binlog_event->length;
      /* Remove the CRC32 from the event length */
      if (con->result->binlog_checksums)
      {
        binlog_event->length-= DRIZZLE_BINLOG_CRC32_LEN;
      }
    }

    /* Check if checksum is correct
     * each event is checksummed individually, the checksum is the last 4 bytes
     * of the binary log event
     * */
    if (con->result->binlog_checksums)
    {
      uint32_t event_crc;
      memcpy(&binlog_event->checksum, binlog_event->raw_data + (binlog_event->raw_length - DRIZZLE_BINLOG_CRC32_LEN), DRIZZLE_BINLOG_CRC32_LEN);
      event_crc= crc32(0, binlog_event->raw_data, (binlog_event->raw_length - DRIZZLE_BINLOG_CRC32_LEN));
      if (event_crc != binlog_event->checksum)
      {
        drizzle_set_error(con, __func__, "CRC doesn't match: 0x%lX, 0x%lX", event_crc, binlog_event->checksum);
        return DRIZZLE_RETURN_BINLOG_CRC;
      }
    }

    if (con->packet_size != 0)
    {
      drizzle_set_error(con, "drizzle_state_binlog_read",
                        "unexpected data after packet:%zu", con->buffer_size);
      return DRIZZLE_RETURN_UNEXPECTED_DATA;
    }
    drizzle_state_pop(con);
  }
  return DRIZZLE_RETURN_OK;
}

