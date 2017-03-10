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
 * @brief Command definitions
 */

#include "config.h"
#include "src/common.h"

/*
 * State Definitions
 */

drizzle_return_t drizzle_state_command_write(drizzle_st *con)
{
  unsigned char *start;
  unsigned char *ptr;
  drizzle_return_t ret;

  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_log_debug(con, __func__);

  if (con->command_data == NULL && con->command_total != 0 &&
      con->command != DRIZZLE_COMMAND_CHANGE_USER)
  {
    return DRIZZLE_RETURN_PAUSE;
  }

  if (con->buffer_size == 0)
  {
    con->buffer_ptr= con->buffer;
    start= con->buffer;
  }
  else
  {
    start= con->buffer_ptr + con->buffer_size;
  }

  if (con->command_offset == 0)
  {
    size_t free_size;

    /* Make sure we can fit the largest non-streaming packet, currently a
       DRIZZLE_COMMAND_CHANGE_USER command. */

    con->packet_size= (uint32_t)(
                      1  /* Command */
                    + strlen(con->user) + 1
                    + 1  /* Scramble size */
                    + DRIZZLE_MAX_SCRAMBLE_SIZE
                    + strlen(con->db) + 1);

    /* Flush buffer if there is not enough room. */
    free_size= con->buffer_allocation - (size_t)(start - con->buffer);
    if (free_size < con->packet_size)
    {
      con->push_state(drizzle_state_write);
      return DRIZZLE_RETURN_OK;
    }

    /* Store packet size at the end since it may change. */
    con->packet_number= 1;
    ptr= start;
    ptr[3]= 0;
    ptr[4]= (unsigned char)(con->command);
    ptr+= 5;

    if (con->command == DRIZZLE_COMMAND_CHANGE_USER)
    {
      ptr= drizzle_pack_auth(con, ptr, &ret);
      if (ret != DRIZZLE_RETURN_OK)
        return ret;

      con->buffer_size+= (4 + con->packet_size);
    }
    else if (con->command_total == 0)
    {
      con->packet_size= 1;
      con->buffer_size+= 5;
    }
    else
    {
      con->packet_size= (uint32_t)(1 + con->command_total);
      free_size-= 5;

      /* Copy as much of the data in as we can into the write buffer. */
      if (con->command_size <= free_size)
      {
        memcpy(ptr, con->command_data, con->command_size);
        con->command_offset= con->command_size;
        con->command_data= NULL;
        con->buffer_size+= 5 + con->command_size;
      }
      else
      {
        memcpy(ptr, con->command_data, free_size);
        con->command_offset= free_size;
        con->command_data+= free_size;
        con->command_size-= free_size;
        con->buffer_size+= 5 + free_size;
      }
    }

    /* Store packet size now. */
    drizzle_set_byte3(start, con->packet_size);
  }
  else
  {
    /* Write directly from the caller buffer for the rest. */
    con->buffer_ptr= con->command_data;
    con->buffer_size= con->command_size;
    con->command_offset+= con->command_size;
    con->command_data= NULL;
  }

  if (con->command_offset == con->command_total)
  {
    con->pop_state();

    if (!(con->state.raw_packet || con->state.no_result_read) &&
        con->command != DRIZZLE_COMMAND_FIELD_LIST)
    {
      con->push_state(drizzle_state_result_read);
      con->push_state(drizzle_state_packet_read);
    }
  }

  con->push_state(drizzle_state_write);

  return DRIZZLE_RETURN_OK;
}
