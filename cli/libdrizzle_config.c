/*
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

#include <glib.h>
#include <stdlib.h>
#include <stdint.h>
#include <glib/gstdio.h>
#include <libdrizzle-5.0/constants.h>
#include "config.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

gboolean show_cflags= FALSE;
gboolean show_include= FALSE;
gboolean show_libs= FALSE;
gboolean show_socket= FALSE;
gboolean show_port= FALSE;
gboolean show_version= FALSE;

static GOptionEntry options[] = {
  { "cflags", 0, 0, G_OPTION_ARG_NONE, &show_cflags, "["ECHO_CFLAGS"]", NULL },
  { "include", 0, 0, G_OPTION_ARG_NONE, &show_include, "[-I"PREFIX"]", NULL },
#ifdef USE_OPENSSL
  { "libs", 0, 0, G_OPTION_ARG_NONE, &show_libs, "[-L"PREFIX" -ldrizzle -lssl]", NULL },
#else
  { "libs", 0, 0, G_OPTION_ARG_NONE, &show_libs, "[-L"PREFIX" -ldrizzle]", NULL },
#endif
  { "socket", 0, 0, G_OPTION_ARG_NONE, &show_socket, "["DRIZZLE_DEFAULT_UDS"]", NULL },
  { "port", 0, 0, G_OPTION_ARG_NONE, &show_port, "["STR(DRIZZLE_DEFAULT_TCP_PORT)"]", NULL },
  { "version", 0, 0, G_OPTION_ARG_NONE, &show_version, "["PACKAGE_VERSION"]", NULL },
  { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL }
};

int main(int argc, char *argv[])
{
  GError *error= NULL;
  GOptionContext *context;
  // Copy argc as it gets reduced when processing
  int old_argc= argc;

  context = g_option_context_new("libdrizzle linking configurator");
  GOptionGroup *main_group= g_option_group_new("main", "Options", "Options", NULL, NULL);
  g_option_group_add_entries(main_group, options);
  g_option_context_set_main_group(context, main_group);
  if (!g_option_context_parse(context, &argc, &argv, &error))
  {
    g_print("option parsing error: %s, try --help\n", error->message);
    return EXIT_FAILURE;
  }
  if (old_argc == 1)
  {
    g_print("%s", g_option_context_get_help(context, TRUE, NULL));
    return EXIT_SUCCESS;
  }
  g_option_context_free(context);
  if (show_version)
  {
    g_print(PACKAGE_VERSION"\n");
  }
  if (show_cflags)
  {
    g_print(ECHO_CFLAGS"\n");
  }
  if (show_include)
  {
    g_print("-I"PREFIX"\n");
  }
  if (show_libs)
  {
#ifdef USE_OPENSSL
    g_print("-L"PREFIX" -ldrizzle -lssl\n");
#else
    g_print("-L"PREFIX" -ldrizzle\n");
#endif
  }
  if (show_socket)
  {
    g_print(DRIZZLE_DEFAULT_UDS"\n");
  }
  if (show_port)
  {
    g_print(STR(DRIZZLE_DEFAULT_TCP_PORT)"\n");
  }
  return EXIT_SUCCESS;
}
