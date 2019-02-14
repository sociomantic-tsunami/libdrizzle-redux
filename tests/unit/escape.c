/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012 Drizzle Developer Group
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

#include <yatl/lite.h>

#include <libdrizzle-redux/libdrizzle.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

int main(int argc, char* argv[])
{
  const char *in = "escape \"this\"\n";

  (void)argc;
  (void)argv;

  char *out;

  // Test invalid parameters
  ASSERT_EQ_(-1, drizzle_escape_string(NULL, &out, NULL, strlen(in)), "from string is NULL");
  ASSERT_EQ_(-1, drizzle_escape_string(NULL, &out, in, 0), "length of from string is 0");

  // Test for data too long
  uint64_t out_len = drizzle_escape_string(NULL, &out, in, strlen(in));
  ASSERT_EQ_(17, out_len, "drizzle_escape_string(): %u != %u", 17,
             (unsigned int)(out_len));
  free(out);

  out_len= drizzle_escape_str(NULL, &out, in, strlen(in), false);
  ASSERT_EQ_(out_len, 17, "Bad hex length output %u", (unsigned int)(out_len));
  ASSERT_EQ_(0, strcmp(out, "escape \\\"this\\\"\\n"), "Bad hex data output");

  free(out);

  const char *with_meta = "This will trip % LIKE _ up";
  out_len = drizzle_escape_str(NULL, &out, with_meta, strlen(with_meta), false);
  ASSERT_EQ_(out_len, 26, "Bad hex length output %u", (unsigned int)(out_len));
  ASSERT_EQ_(0, strcmp(out, with_meta), "Bad hex data output");
  free(out);

  out_len = drizzle_escape_str(NULL, &out, with_meta, strlen(with_meta), true);
  ASSERT_EQ_(out_len, 28, "Bad hex length output %u", (unsigned int)(out_len));
  ASSERT_EQ_(0, strcmp(out, "This will trip \\% LIKE \\_ up"), "Bad hex data output");
  free(out);

  const char *tricky = "This\tis \\\\\b tricky";
  out_len = drizzle_escape_str(NULL, &out, tricky, strlen(tricky), false);
  ASSERT_EQ_(out_len, 22, "Bad hex length output %u", (unsigned int)(out_len));
  ASSERT_EQ_(0, strcmp(out, "This\\tis \\\\\\\\\\b tricky"), "Bad hex data output");
  free(out);

  const char *str = "backslash, zero, ctrl char, return carriage, space, \
    single quote \\, 0, \r \032 \'";
  out_len = drizzle_escape_str(NULL, &out, str, strlen(str), false);
  ASSERT_EQ_(out_len, 84, "Bad hex length output %u", (unsigned int)(out_len));
  free(out);

  return EXIT_SUCCESS;
}
