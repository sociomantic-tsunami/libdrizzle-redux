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
 * @brief State machine definitions
 */

#include "config.h"
#include "libdrizzle/common.h"

drizzle_return_t drizzle_state_loop(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  while (con->has_state() == false)
  {
    drizzle_return_t ret= con->current_state();
    if (ret != DRIZZLE_RETURN_OK)
    {
      if (ret != DRIZZLE_RETURN_IO_WAIT && ret != DRIZZLE_RETURN_PAUSE &&
          ret != DRIZZLE_RETURN_ERROR_CODE)
      {
        drizzle_close(con);
      }

      return ret;
    }
  }

  return DRIZZLE_RETURN_OK;
}

drizzle_return_t drizzle_state_packet_read(drizzle_st *con)
{
  if (con == NULL)
  {
    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

  drizzle_log_debug(con, __func__);

  if (con->buffer_size < 4)
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  con->packet_size= drizzle_get_byte3(con->buffer_ptr);

  if (con->buffer_size < (con->packet_size + 4))
  {
    con->push_state(drizzle_state_read);
    return DRIZZLE_RETURN_OK;
  }

  if (con->packet_number != con->buffer_ptr[3])
  {
    drizzle_set_error(con, __func__,
                      "bad packet number:%u:%u", con->packet_number,
                      con->buffer_ptr[3]);
    return DRIZZLE_RETURN_BAD_PACKET_NUMBER;
  }

  drizzle_log_debug(con, "buffer_size= %zu, packet_size= %zu, packet_number= %u",
                    con->buffer_size, con->packet_size, con->packet_number);

  con->packet_number++;

  con->buffer_ptr+= 4;
  con->buffer_size-= 4;

  con->pop_state();

  return DRIZZLE_RETURN_OK;
}
