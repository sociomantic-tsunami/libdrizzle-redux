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

#include <zlib.h>
#include <inttypes.h>

drizzle_binlog_st *drizzle_binlog_init(drizzle_st *con,
                                       drizzle_binlog_fn *binlog_fn,
                                       drizzle_binlog_error_fn *error_fn,
                                       void *context,
                                       bool verify_checksums)
{
  if (con == NULL)
  {
    return NULL;
  }

  drizzle_binlog_st *binlog= new (std::nothrow) drizzle_binlog_st;
  if (binlog == NULL)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "error allocating binlog struct");
    return NULL;
  }
  binlog->con= con;
  binlog->binlog_fn= binlog_fn;
  binlog->error_fn= error_fn;
  binlog->binlog_context= context;
  binlog->verify_checksums= verify_checksums;

  return binlog;
}

void drizzle_binlog_free(drizzle_binlog_st *binlog)
{
  delete binlog;
}

drizzle_return_t drizzle_binlog_start(drizzle_binlog_st *binlog,
                                          uint32_t server_id,
                                          const char *file,
                                          uint32_t start_position)
{
  unsigned char data[128];
  unsigned char *ptr;
  uint8_t len= 0, fn_len= 0;
  drizzle_result_st *result;
  drizzle_st *con;
  drizzle_return_t ret;

  if (binlog == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  con= binlog->con;

  // Hack in 5.6 to say that client support checksums
  result= drizzle_query(con, "SET @master_binlog_checksum='NONE'", 0, &ret);
  drizzle_result_free(result);

  // Ensure the checksum query is executed if we are in non-blocking mode
  if (con->options.non_blocking)
  {
    drizzle_wait(con);
    ret = drizzle_state_loop(con);
  }

  if (ret != DRIZZLE_RETURN_OK)
  {
    return ret;
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

  result= drizzle_command_write(con, NULL, DRIZZLE_COMMAND_BINLOG_DUMP,
                                   data, len, len, &ret);

  con->binlog= binlog;

  if (con->options.non_blocking)
  {
    // In non-blocking node, wait for IO but free the result as data is
    // expected to be read by the client
    ret = drizzle_wait(con);
    drizzle_result_free(result);
  }

  if (ret != DRIZZLE_RETURN_OK)
  {
    return ret;
  }

  if (con->options.socket_owner == DRIZZLE_SOCKET_OWNER_NATIVE)
  {
    result->push_state(drizzle_state_binlog_read);
    result->push_state(drizzle_state_packet_read);
  }

  return drizzle_state_loop(con);
}

uint32_t drizzle_binlog_event_timestamp(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return 0;
  }

  return event->timestamp;
}

drizzle_binlog_event_types_t drizzle_binlog_event_type(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return drizzle_binlog_event_types_t();
  }

  return event->type;
}

uint32_t drizzle_binlog_event_server_id(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return 0;
  }

  return event->server_id;
}

uint32_t drizzle_binlog_event_length(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return 0;
  }

  return event->length;
}

uint32_t drizzle_binlog_event_next_pos(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return 0;
  }

  return event->next_pos;
}

uint16_t drizzle_binlog_event_flags(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return 0;
  }

  return event->flags;
}

uint16_t drizzle_binlog_event_extra_flags(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return 0;
  }

  return event->extra_flags;
}

const unsigned char *drizzle_binlog_event_data(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return NULL;
  }

  return event->data;
}

const unsigned char *drizzle_binlog_event_raw_data(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return NULL;
  }

  return event->raw_data;
}

uint32_t drizzle_binlog_event_raw_length(drizzle_binlog_event_st *event)
{
  if (event == NULL)
  {
    return 0;
  }

  return event->raw_length;
}

drizzle_return_t drizzle_state_binlog_read(drizzle_st *con)
{
  drizzle_binlog_event_st *binlog_event;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  binlog_event= &con->binlog->event;

  if (con->packet_size != 0 && con->buffer_size < con->packet_size)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (con->packet_size == 5 && con->buffer_ptr[0] == 254)
  {
    /* Got EOF packet, no more data. */
    con->result->warning_count= drizzle_get_byte2(con->buffer_ptr + 1);
    con->status= (drizzle_status_t)drizzle_get_byte2(con->buffer_ptr + 3);
    con->buffer_ptr+= 5;
    con->buffer_size-= 5;
    con->pop_state();
    con->binlog->error_fn(DRIZZLE_RETURN_EOF, con, con->binlog->binlog_context);
    return DRIZZLE_RETURN_EOF;
  }
  else if (drizzle_check_unpack_error(con))
  {
    snprintf(con->result->info, DRIZZLE_MAX_INFO_SIZE, "%.*s",
             (int)con->packet_size, con->last_error);
    con->result->info[DRIZZLE_MAX_INFO_SIZE-1]= 0;

    con->pop_state();
    con->binlog->error_fn(DRIZZLE_RETURN_ERROR_CODE, con, con->binlog->binlog_context);
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
        drizzle_set_error(con, __FILE_LINE_FUNC__, "packet size error:%" PRIu32
                          ":%" PRIu32, con->packet_size, binlog_event->length);
        con->binlog->error_fn(DRIZZLE_RETURN_UNEXPECTED_DATA, con, con->binlog->binlog_context);
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
      /* 5.6.1 or higher is automatic checksums on */
      if (binlog_event->type == DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION)
      {
        if (strncmp((const char*)con->buffer_ptr + 2, DRIZZLE_BINLOG_CHECKSUM_VERSION, strlen(DRIZZLE_BINLOG_CHECKSUM_VERSION)) <= 0)
        {
          con->binlog->has_checksums= true;
        }
      }
      /* A checksum is basically a CRC32 at the end of the event data (4 bytes) */
      binlog_event->data= con->buffer_ptr;
      con->buffer_ptr+= binlog_event->length;
      con->buffer_size-= binlog_event->length;
      con->packet_size-= binlog_event->length;
      /* Remove the CRC32 from the event length */
      if (con->binlog->has_checksums)
      {
        binlog_event->length-= DRIZZLE_BINLOG_CRC32_LEN;
      }
    }

    /* Check if checksum is correct
     * each event is checksummed individually, the checksum is the last 4 bytes
     * of the binary log event
     * */
    if (con->binlog->has_checksums)
    {
      uint32_t event_crc;
      memcpy(&binlog_event->checksum, binlog_event->raw_data + (binlog_event->raw_length - DRIZZLE_BINLOG_CRC32_LEN), DRIZZLE_BINLOG_CRC32_LEN);
      if (con->binlog->verify_checksums)
      {
        event_crc= (uint32_t)crc32(0, binlog_event->raw_data, (binlog_event->raw_length - DRIZZLE_BINLOG_CRC32_LEN));
        if (event_crc != binlog_event->checksum)
        {
          drizzle_set_error(con, __FILE_LINE_FUNC__, "CRC doesn't match: 0x%"
            PRIX32 ", 0x%" PRIX32, event_crc, binlog_event->checksum);
          con->binlog->error_fn(DRIZZLE_RETURN_BINLOG_CRC, con, con->binlog->binlog_context);
          return DRIZZLE_RETURN_BINLOG_CRC;
        }
      }
    }

    if (con->packet_size != 0)
    {
      drizzle_set_error(con, __FILE_LINE_FUNC__,
                        "unexpected data after packet:%" PRIu64, con->buffer_size);
      con->binlog->error_fn(DRIZZLE_RETURN_UNEXPECTED_DATA, con, con->binlog->binlog_context);
      return DRIZZLE_RETURN_UNEXPECTED_DATA;
    }
    con->pop_state();
  }

  con->binlog->binlog_fn(&con->binlog->event, con->binlog->binlog_context);
  con->push_state(drizzle_state_binlog_read);
  con->push_state(drizzle_state_packet_read);

  return DRIZZLE_RETURN_OK;
}

const char *drizzle_binlog_event_type_str(drizzle_binlog_event_types_t event_type)
{
    switch(event_type)
    {
      case DRIZZLE_EVENT_TYPE_UNKNOWN : return "DRIZZLE_EVENT_TYPE_UNKNOWN";
      case DRIZZLE_EVENT_TYPE_START : return "DRIZZLE_EVENT_TYPE_START";
      case DRIZZLE_EVENT_TYPE_QUERY : return "DRIZZLE_EVENT_TYPE_QUERY";
      case DRIZZLE_EVENT_TYPE_STOP : return "DRIZZLE_EVENT_TYPE_STOP";
      case DRIZZLE_EVENT_TYPE_ROTATE : return "DRIZZLE_EVENT_TYPE_ROTATE";
      case DRIZZLE_EVENT_TYPE_INTVAR : return "DRIZZLE_EVENT_TYPE_INTVAR";
      case DRIZZLE_EVENT_TYPE_LOAD : return "DRIZZLE_EVENT_TYPE_LOAD";
      case DRIZZLE_EVENT_TYPE_SLAVE : return "DRIZZLE_EVENT_TYPE_SLAVE";
      case DRIZZLE_EVENT_TYPE_CREATE_FILE : return "DRIZZLE_EVENT_TYPE_CREATE_FILE";
      case DRIZZLE_EVENT_TYPE_APPEND_BLOCK : return "DRIZZLE_EVENT_TYPE_APPEND_BLOCK";
      case DRIZZLE_EVENT_TYPE_EXEC_LOAD : return "DRIZZLE_EVENT_TYPE_EXEC_LOAD";
      case DRIZZLE_EVENT_TYPE_DELETE_FILE : return "DRIZZLE_EVENT_TYPE_DELETE_FILE";
      case DRIZZLE_EVENT_TYPE_NEW_LOAD : return "DRIZZLE_EVENT_TYPE_NEW_LOAD";
      case DRIZZLE_EVENT_TYPE_RAND : return "DRIZZLE_EVENT_TYPE_RAND";
      case DRIZZLE_EVENT_TYPE_USER_VAR : return "DRIZZLE_EVENT_TYPE_USER_VAR";
      case DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION : return "DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION";
      case DRIZZLE_EVENT_TYPE_XID : return "DRIZZLE_EVENT_TYPE_XID";
      case DRIZZLE_EVENT_TYPE_BEGIN_LOAD_QUERY : return "DRIZZLE_EVENT_TYPE_BEGIN_LOAD_QUERY";
      case DRIZZLE_EVENT_TYPE_EXECUTE_LOAD_QUERY : return "DRIZZLE_EVENT_TYPE_EXECUTE_LOAD_QUERY";
      case DRIZZLE_EVENT_TYPE_TABLE_MAP : return "DRIZZLE_EVENT_TYPE_TABLE_MAP";
        // Next 3 are used in 5.1.0 -> 5.1.15 only
      case DRIZZLE_EVENT_TYPE_OBSOLETE_WRITE_ROWS : return "DRIZZLE_EVENT_TYPE_OBSOLETE_WRITE_ROWS";
      case DRIZZLE_EVENT_TYPE_OBSOLETE_UPDATE_ROWS : return "DRIZZLE_EVENT_TYPE_OBSOLETE_UPDATE_ROWS";
      case DRIZZLE_EVENT_TYPE_OBSOLETE_DELETE_ROWS : return "DRIZZLE_EVENT_TYPE_OBSOLETE_DELETE_ROWS";
      case DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS : return "DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS";
      case DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS : return "DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS";
      case DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS : return "DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS";
      case DRIZZLE_EVENT_TYPE_INCIDENT : return "DRIZZLE_EVENT_TYPE_INCIDENT";
      case DRIZZLE_EVENT_TYPE_HEARTBEAT : return "DRIZZLE_EVENT_TYPE_HEARTBEAT";
      case DRIZZLE_EVENT_TYPE_IGNORABLE : return "DRIZZLE_EVENT_TYPE_IGNORABLE";
      case DRIZZLE_EVENT_TYPE_ROWS_QUERY : return "DRIZZLE_EVENT_TYPE_ROWS_QUERY";
      case DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS : return "DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS";
      case DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS : return "DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS";
      case DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS : return "DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS";
      case DRIZZLE_EVENT_TYPE_GTID : return "DRIZZLE_EVENT_TYPE_GTID";
      case DRIZZLE_EVENT_TYPE_ANONYMOUS_GTID : return "DRIZZLE_EVENT_TYPE_ANONYMOUS_GTID";
      case DRIZZLE_EVENT_TYPE_PREVIOUS_GTIDS : return "DRIZZLE_EVENT_TYPE_PREVIOUS_GTIDS";
      case DRIZZLE_EVENT_TYPE_END : return "DRIZZLE_EVENT_TYPE_END";
      default : return "DRIZZLE_EVENT_TYPE_UNKNOWN";
    }
}

drizzle_return_t drizzle_binlog_get_filename(drizzle_st *con, char **filename,
                                             uint32_t *end_position, int file_index)
{
  *filename = (char*) malloc(1);
  *filename[0] = '\0';
  *end_position = 0;
  drizzle_return_t driz_ret;

  drizzle_result_st *result = drizzle_query(con, "SHOW BINARY LOGS", 0, &driz_ret);

  __LOG_LOCATION__

  if (driz_ret != DRIZZLE_RETURN_OK)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "Query to retrieve binary logs failed");
    drizzle_result_free(result);
    return driz_ret;
  }

  drizzle_result_buffer(result);
  int row_count = (int) drizzle_result_row_count(result);

  if (file_index < -1 || file_index >= row_count)
  {
    drizzle_set_error(con, __FILE_LINE_FUNC__, "Invalid binlog file index: %d", file_index);
    drizzle_result_free(result);
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (row_count > 0)
  {
    int row_index = file_index == -1 || file_index >= row_count ?
                    row_count - 1 : file_index;

    drizzle_row_t row = drizzle_row_index(result, row_index);
    *filename = (char*)realloc(*filename, strlen(row[0]) + 1);
    memcpy(*filename, row[0], strlen(row[0]));
    (*filename)[strlen(row[0])] = '\0';

    *end_position = atoi(row[1]);
    drizzle_log_info(con, __FILE_LINE_FUNC__, "Found binary log '%s' with filesize %d bytes",
                     *filename, *end_position);
  }
  else
  {
    drizzle_log_info(con, __FILE_LINE_FUNC__, "No binary log files found");
  }

  drizzle_result_free(result);
  return DRIZZLE_RETURN_OK;
}
