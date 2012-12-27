/*  vim:expandtab:shiftwidth=2:tabstop=2:smarttab: 
 *
 *  Drizzle Client & Protocol Library
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

#include <libdrizzle-5.1/libdrizzle.h>
#include "libdrizzle/structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>

#ifndef EXIT_SKIP
# define EXIT_SKIP 77
#endif

int main(int argc, char *argv[])
{
  (void) argc;
  (void) argv;
  drizzle_st *con;
  drizzle_return_t ret;
  drizzle_result_st *result;

  con = drizzle_create_tcp("localhost", 3306, "root", "", "", 0);
  if (con == NULL)
  {
    printf("Drizzle connection object creation error\n");
    return EXIT_FAILURE;
  }
  ret = drizzle_connect(con);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drizzle connection failure\n");
    drizzle_quit(con);
    return EXIT_SKIP;
  }

  result= drizzle_start_binlog(con, 0, "", 0, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drizzle binlog start failure\n");
    return EXIT_FAILURE;
  }

  while (ret == DRIZZLE_RETURN_OK)
  {
    uint32_t timestamp;
    ret= drizzle_binlog_get_next_event(result);
    if (ret != DRIZZLE_RETURN_OK)
      break;
    timestamp= drizzle_binlog_event_timestamp(result);
    /* Test to see if timestamp is greater than 2012-01-01 00:00:00, corrupted
     * timestamps will have weird values that shoud fail this after several
     * events.  Also rotate event doesn't have a timestamp so need to add 0
     * to this test */
    if ((timestamp < 1325376000) && (timestamp != 0))
    {
      printf("Bad timestamp retrieved: %"PRIu32"\n", timestamp);
      return EXIT_FAILURE;
    }
    /* An event higher than the max known is bad, either we don't know about
     * new events or type is corrupted */
    if (drizzle_binlog_event_type(result) >= DRIZZLE_EVENT_TYPE_END)
    {
      printf("Bad event type: %d\n", drizzle_binlog_event_type(result));
      return EXIT_FAILURE;
    }
  }

  drizzle_quit(con);
  return EXIT_SUCCESS;
}
