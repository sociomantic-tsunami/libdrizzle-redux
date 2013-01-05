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

#include <libdrizzle-5.1/libdrizzle.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef HAVE_PWD_H
# include <pwd.h>
#endif

#include <unistd.h>
#include <errno.h>
#include <time.h>

#ifdef HAVE_ARGP_H
# include <argp.h>
#else
# error "drizzle_binlogs requires <argp.h>"
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

char *host= NULL;
uint16_t port= DRIZZLE_DEFAULT_TCP_PORT;
char *user= NULL;
bool user_alloced= false;
char *pass= NULL;
char *outdir= NULL;
char *start_file= NULL;
uint32_t start_pos= 0;
bool continuous= false;
uint8_t opt_count= 0;

typedef enum
{
  OPT_NONE,
  OPT_HOST,
  OPT_PORT,
  OPT_USER,
  OPT_PASS,
  OPT_OUTDIR,
  OPT_FILE,
  OPT_START_POS,
  OPT_CONTINUOUS
} option_consts_t;

static struct argp_option options[]= {
  { "host", OPT_HOST, "HOST", 0, "Hostname of server, default "DRIZZLE_DEFAULT_TCP_HOST, 0 },
  { "port", OPT_PORT, "PORT", 0, "Port number of server, default "STR(DRIZZLE_DEFAULT_TCP_PORT), 0 },
  { "user", OPT_USER, "USER", 0, "Username for the server, default is current system user", 0 },
  { "pass", OPT_PASS, "PASS", 0, "Password for the server", 0 },
  { "outdir", OPT_OUTDIR, "OUTDIR", 0, "Output directory", 0 },
  { 0, 0, 0, 0, "Binlog Options", 1 },
  { "start-file", OPT_FILE, "FILENAME", 0, "Binlog file to start with", 1 },
  { "start-pos", OPT_START_POS, "POS", 0, "Position to start with", 1 },
  { "continuous", OPT_CONTINUOUS, 0, 0, "Continous download mode", 1 },
  { 0 }
};

static char doc[]= "Drizzle binlog retriever";
static char args_doc[]= "";
static error_t parse_opt(int key, char *arg, struct argp_state *state)
{
  (void)(state);
  switch(key)
  {
    case OPT_HOST:
      host= arg;
      opt_count++;
      break;
    case OPT_PORT:
      port= atoi(arg);
      opt_count++;
      break;
    case OPT_USER:
      user= arg;
      opt_count++;
      break;
    case OPT_PASS:
      pass= arg;
      opt_count++;
      break;
    case OPT_OUTDIR:
      outdir= arg;
      opt_count++;
      break;
    case OPT_FILE:
      start_file= arg;
      opt_count++;
      break;
    case OPT_START_POS:
      start_pos= atoi(arg);
      opt_count++;
      break;
    case OPT_CONTINUOUS:
      continuous= true;
      opt_count++;
      break;
    case ARGP_KEY_NO_ARGS:
      break;
    default:
      return ARGP_ERR_UNKNOWN;
      break;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc, NULL, NULL, NULL };

bool get_system_user(char *dest, uint8_t len);
drizzle_st *_connect(void);
FILE *create_binlog_file(char *binlog_file);
void get_binlogs(drizzle_st *con);
void write_binlog(FILE* file, const unsigned char* data, uint32_t len);

bool get_system_user(char *dest, uint8_t len)
{
  long pw_len= sysconf(_SC_GETPW_R_SIZE_MAX);
  struct passwd pw_struct;
  struct passwd *pw_tmp_struct;
  if (pw_len == -1)
  {
    pw_len= 1024;
  }
  char *pw_buffer= malloc(pw_len);

  if (getpwuid_r(geteuid(), &pw_struct, pw_buffer, pw_len, &pw_tmp_struct) == 0)
  {
    strncpy(dest, pw_struct.pw_name, len);
    free(pw_buffer);
    return true;
  }
  free(pw_buffer);
  return false;
}

drizzle_st *_connect(void)
{
  drizzle_st *con;
  drizzle_return_t ret;

  con= drizzle_create_tcp(host, port, user, pass, "", 0);
  if (!con)
  {
    printf("Drizzle connection object creation error\n");
    return NULL;
  }
  ret= drizzle_connect(con);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Error connecting to server: %s\n", drizzle_error(con));
    return NULL;
  }
  return con;
}

FILE *create_binlog_file(char *binlog_file)
{
  FILE *outfile;
  char filename[PATH_MAX];
  if (outdir)
  {
    snprintf(filename, PATH_MAX, "%s/%s", outdir, binlog_file);
    outfile= fopen(filename, "w");
  }
  else
  {
    outfile= fopen(binlog_file, "w");
  }

  return outfile;
}

void get_binlogs(drizzle_st *con)
{
  drizzle_result_st *result;
  drizzle_return_t ret;
  uint16_t server_id;
  FILE *outfile;
  char binlog_file[PATH_MAX];
  uint32_t event_len;
  bool read_end= false;

  if (continuous)
  {
    srand(time(NULL));
    // Random server ID from range 32767 - 65535
    server_id= rand() % 32768 + 32767;
  }
  else
  {
    server_id= 0;
  }

  result= drizzle_start_binlog(con, server_id, start_file, start_pos, &ret);
  if (ret != DRIZZLE_RETURN_OK)
  {
    printf("Drizzle binlog start failure: %s\n", drizzle_error(con));
    exit(EXIT_FAILURE);
  }

  snprintf(binlog_file, PATH_MAX, "%s", start_file);
  outfile= create_binlog_file(binlog_file);
  if (!outfile)
  {
    printf("Could not create binlog file '%s', errno %d\n", binlog_file, errno);
    exit(EXIT_FAILURE);
  }

  while(1)
  {
    write_binlog(outfile, (unsigned char *)DRIZZLE_BINLOG_MAGIC, 4);
    while(1)
    {
      ret= drizzle_binlog_get_next_event(result);
      event_len= drizzle_binlog_event_raw_length(result);
      if (ret != DRIZZLE_RETURN_OK)
      {
          // EOF
          if (ret != DRIZZLE_RETURN_EOF)
          {
            printf("Read error: %d - %s\n", ret, drizzle_error(con));
          }
          read_end= true;
          break;
      }
      if (drizzle_binlog_event_type(result) == DRIZZLE_EVENT_TYPE_ROTATE)
      {
        fclose(outfile);
        snprintf(binlog_file, PATH_MAX, "%.*s", drizzle_binlog_event_length(result), drizzle_binlog_event_data(result));
        outfile= create_binlog_file(binlog_file);
        if (!outfile)
        {
          printf("Could not create binlog file '%s', errno %d\n", binlog_file, errno);
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

void write_binlog(FILE* file, const unsigned char* data, uint32_t len)
{
  if (len)
  {
    if (write(fileno(file), data, len) <= 0)
    {
      printf("Error: binlog: Error writing binary log: %s", strerror(errno));
      exit(EXIT_FAILURE);
    }
  }

}

int main(int argc, char *argv[])
{
  drizzle_st *con;
  char sysuser[DRIZZLE_MAX_USER_SIZE];
  // Use the define here because we want it fixed at compile time
  argp_program_version= LIBDRIZZLE_VERSION_STRING;
  argp_program_bug_address= drizzle_bugreport();
  argp_parse(&argp, argc, argv, 0, 0, NULL);
  if (opt_count == 0)
  {
    argp_help(&argp, stdout, ARGP_HELP_STD_USAGE, program_invocation_short_name);
    return EXIT_FAILURE;
  }
  if (!user)
  {
    user= sysuser;
    if (!get_system_user(user, DRIZZLE_MAX_USER_SIZE))
    {
      printf("No user specified and could not determine current user\n");
      return EXIT_FAILURE;
    }
  }
  if (!host)
  {
    host= (char*)DRIZZLE_DEFAULT_TCP_HOST;
  }
  if (!port)
  {
    port= DRIZZLE_DEFAULT_TCP_PORT;
  }
  if (!start_file)
  {
    printf("Binlog start file required\n");
    return EXIT_FAILURE;
  }
  con = _connect();
  if (!con)
  {
    return EXIT_FAILURE;
  }
  get_binlogs(con);
  drizzle_quit(con);
  return EXIT_SUCCESS;
}
