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

#include "config.h"
#include <libdrizzle-5.0/libdrizzle.h>
#include <stdlib.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <pwd.h>
#include <unistd.h>

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

gchar *host= NULL;
gint port= DRIZZLE_DEFAULT_TCP_PORT;
gchar *user= NULL;
gboolean user_alloced= FALSE;
gchar *pass= NULL;
gchar *outdir= NULL;
gchar *start_file= NULL;
gint start_pos= 0;
gboolean continuous= FALSE;

static GOptionEntry main_options[]= {
  { "host", 0, 0, G_OPTION_ARG_STRING, &host, "Hostname of server, default "DRIZZLE_DEFAULT_TCP_HOST, NULL },
  { "port", 0, 0, G_OPTION_ARG_INT, &port, "Port number of server, default "STR(DRIZZLE_DEFAULT_TCP_PORT), NULL },
  { "user", 0, 0, G_OPTION_ARG_STRING, &user, "Username for the server, default is current system user", NULL },
  { "pass", 0, 0, G_OPTION_ARG_STRING, &pass, "Password for the server", NULL },
  { "outdir", 0, 0, G_OPTION_ARG_FILENAME, &outdir, "Output directory", NULL },
  { NULL, 0, 0, G_OPTION_ARG_NONE, NULL, NULL, NULL }
};

static GOptionEntry binlog_options[]= {
  { "start-file", 0, 0, G_OPTION_ARG_FILENAME, &start_file, "Binlog file to start with", NULL },
  { "start-pos", 0, 0, G_OPTION_ARG_INT, &start_pos, "Position to start with", NULL },
  { "continuous", 0, 0, G_OPTION_ARG_NONE, &continuous, "Continous download mode", NULL }
};

gboolean get_system_user(char *dest, uint8_t len);
drizzle_con_st *_connect(void);
FILE *create_binlog_file(char *binlog_file);
void get_binlogs(drizzle_con_st *con);
void write_binlog(FILE* file, const uint8_t* data, uint32_t len);

gboolean get_system_user(char *dest, uint8_t len)
{
  long pw_len= sysconf(_SC_GETPW_R_SIZE_MAX);
  struct passwd pw_struct;
  struct passwd *pw_tmp_struct;
  if (pw_len == -1)
  {
    pw_len= 1024;
  }
  char *pw_buffer= g_malloc(pw_len);

  if (getpwuid_r(geteuid(), &pw_struct, pw_buffer, pw_len, &pw_tmp_struct) == 0)
  {
    g_strlcpy(dest, pw_struct.pw_name, len);
    g_free(pw_buffer);
    return TRUE;
  }
  g_free(pw_buffer);
  return FALSE;
}

drizzle_con_st *_connect(void)
{
  drizzle_st *drizzle;
  drizzle_con_st *con;
  drizzle_return_t ret;

  drizzle= drizzle_create();
  if (!drizzle)
  {
    g_print("Drizzle object creation error\n");
    return NULL;
  }
  con= drizzle_con_add_tcp(drizzle, host, port, user, pass, "", 0);
  if (!con)
  {
    g_print("Drizzle connection object creation error\n");
    return NULL;
  }
  ret= drizzle_con_connect(con);
  if (ret != DRIZZLE_RETURN_OK)
  {
    g_print("Error connecting to server: %s\n", drizzle_con_error(con));
    return NULL;
  }
  return con;
}

FILE *create_binlog_file(char *binlog_file)
{
  FILE *outfile;
  char *filename;
  if (outdir)
  {
    filename= g_strdup_printf("%s/%s", outdir, binlog_file);
  }
  else
  {
    filename= binlog_file;
  }
  outfile= g_fopen(filename, "w");
  if (outdir)
  {
    g_free(filename);
  }
  return outfile;
}

void get_binlogs(drizzle_con_st *con)
{
  drizzle_result_st *result;
  drizzle_return_t ret;
  int server_id;
  FILE *outfile;
  gchar *binlog_file;
  uint32_t event_len;
  gboolean read_end= FALSE;

  if (continuous)
  {
    server_id= g_random_int_range(32768, 65535);
  }
  else
  {
    server_id= 0;
  }

  result= drizzle_start_binlog(con, server_id, start_file, start_pos, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    g_print("Drizzle binlog start failure: %s\n", drizzle_con_error(con));
    exit(EXIT_FAILURE);
  }

  binlog_file= g_strdup(start_file);
  outfile= create_binlog_file(binlog_file);
  if (!outfile)
  {
    g_print("Could not create binlog file '%s', errno %d\n", binlog_file, errno);
    exit(EXIT_FAILURE);
  }

  while(1)
  {
    write_binlog(outfile, (uint8_t *)DRIZZLE_BINLOG_MAGIC, 4);
    while(1)
    {
      ret= drizzle_binlog_get_next_event(result);
      event_len= drizzle_binlog_event_raw_length(result);
      if (ret != DRIZZLE_RETURN_OK)
      {
          // EOF
          if (ret != DRIZZLE_RETURN_EOF)
          {
            g_print("Read error: %d - %s\n", ret, drizzle_con_error(con));
          }
          read_end= TRUE;
          break;
      }
      if (drizzle_binlog_event_type(result) == DRIZZLE_EVENT_TYPE_ROTATE)
      {
        fclose(outfile);
        g_free(binlog_file);
        binlog_file= g_strndup((const gchar *)drizzle_binlog_event_data(result), drizzle_binlog_event_length(result));
        outfile= create_binlog_file(binlog_file);
        if (!outfile)
        {
          g_print("Could not create binlog file '%s', errno %d\n", binlog_file, errno);
          exit(EXIT_FAILURE);
        }
        break;
      }
      write_binlog(outfile, drizzle_binlog_event_raw_data(result), event_len);
    }
    if (read_end)
    {
      break;
    }
  }

  drizzle_result_free(result);
}

void write_binlog(FILE* file, const uint8_t* data, uint32_t len)
{
  if (len)
  {
    if (write(fileno(file), data, len) <= 0)
    {
      g_print("Error: binlog: Error writing binary log: %s", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

}

int main(int argc, char *argv[])
{
  GError *error= NULL;
  GOptionContext *context;
  drizzle_st *drizzle;
  drizzle_con_st *con;
  char sysuser[DRIZZLE_MAX_USER_SIZE];

  context= g_option_context_new(" - Drizzle binlog retriever");
  GOptionGroup *main_group= g_option_group_new("main", "Main Options", "Main Options", NULL, NULL);
  GOptionGroup *binlog_group= g_option_group_new("binlog", "Binlog Options", "Binlog Options", NULL, NULL);
  g_option_group_add_entries(main_group, main_options);
  g_option_group_add_entries(binlog_group, binlog_options);
  g_option_context_set_main_group(context, main_group);
  g_option_context_add_group(context, binlog_group);
  if (!g_option_context_parse(context, &argc, &argv, &error))
  {
    g_print("Error parsing options: %s, try --help\n", error->message);
    return EXIT_FAILURE;
  }
  if (!user)
  {
    user= sysuser;
    if (!get_system_user(user, DRIZZLE_MAX_USER_SIZE))
    {
      g_print("No user specified and could not determine current user\n");
      return EXIT_FAILURE;
    }
  }
  if (!host)
  {
    host= g_strdup(DRIZZLE_DEFAULT_TCP_HOST);
  }
  if (!port)
  {
    port= DRIZZLE_DEFAULT_TCP_PORT;
  }
  if (!start_file)
  {
    g_print("Binlog start file required\n");
    return EXIT_FAILURE;
  }
  con = _connect();
  if (!con)
  {
    return EXIT_FAILURE;
  }
  get_binlogs(con);
  drizzle= drizzle_con_drizzle(con);
  drizzle_con_quit(con);
  drizzle_free(drizzle);
  g_option_context_free(context);
  return EXIT_SUCCESS;
}
