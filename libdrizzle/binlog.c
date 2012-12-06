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

#include <libdrizzle/common.h>


drizzle_result_st *drizzle_start_binlog(drizzle_con_st *con,
                                            uint32_t server_id,
                                            const char *file,
                                            uint32_t start_position,
                                            drizzle_return_t *ret_ptr)
{ 
  uint8_t data[128];
  uint8_t *ptr;
  uint8_t len= 0, fn_len= 0;

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

  return drizzle_con_command_write(con, NULL, DRIZZLE_COMMAND_BINLOG_DUMP,
                                   data, len, len, ret_ptr);
}

drizzle_binlog_st *drizzle_binlog_get_event(drizzle_result_st *result,
                                            drizzle_return_t *ret_ptr)
{
  drizzle_return_t unused;
  if (ret_ptr == NULL)
  {
    ret_ptr= &unused;
  }

  if (result == NULL)
  {
    *ret_ptr= DRIZZLE_RETURN_INVALID_ARGUMENT;
    return NULL;
  }

  drizzle_state_push(result->con, drizzle_state_binlog_read);
  drizzle_state_push(result->con, drizzle_state_packet_read);
  *ret_ptr= drizzle_state_loop(result->con);

  return result->binlog_event;
}

drizzle_return_t drizzle_state_binlog_read(drizzle_con_st *con)
{
  drizzle_binlog_st *binlog_event;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  if (con->result->binlog_event == NULL)
  {
    con->result->binlog_event = malloc(sizeof(drizzle_binlog_st));
    con->result->binlog_event->data= NULL;
  }
  binlog_event= con->result->binlog_event;

  if (con->packet_size != 0 && con->buffer_size < con->packet_size &&
      con->buffer_size < 5)
  {
    drizzle_state_push(con, drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (con->packet_size == 5 && con->buffer_ptr[0] == 254)
  {
    /* Got EOF packet, no more rows. */
    con->result->row_current= 0;
    con->result->warning_count= drizzle_get_byte2(con->buffer_ptr + 1);
    con->status= drizzle_get_byte2(con->buffer_ptr + 3);
    con->buffer_ptr+= 5;
    con->buffer_size-= 5;
    drizzle_state_pop(con);
    return DRIZZLE_RETURN_EOF;
  }
  else
  {
    con->buffer_ptr++;
    con->packet_size--;
    con->buffer_size--;
    binlog_event->timestamp= drizzle_get_byte4(con->buffer_ptr);
    binlog_event->type= con->buffer_ptr[4];
    binlog_event->server_id= drizzle_get_byte4(con->buffer_ptr + 5);
    binlog_event->length= drizzle_get_byte4(con->buffer_ptr + 9);
    if (binlog_event->length == 19)
    {
      binlog_event->length= 0;
      binlog_event->next_pos= drizzle_get_byte4(con->buffer_ptr + 13);
      binlog_event->flags= drizzle_get_byte2(con->buffer_ptr + 17);
      con->buffer_ptr+= 19;
      con->buffer_size-= 19;
      con->packet_size-= 19;
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
      binlog_event->data= realloc(binlog_event->data, binlog_event->length);
      memcpy(binlog_event->data, con->buffer_ptr, binlog_event->length);
      con->buffer_ptr+= binlog_event->length;
      con->buffer_size-= binlog_event->length;
      con->packet_size-= binlog_event->length;
    }
    drizzle_state_pop(con);
  }
  return DRIZZLE_RETURN_OK;
}
