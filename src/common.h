/*
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
 * @brief System Include Files
 */

#pragma once

#include <libdrizzle-5.1/drizzle_client.h>

#include <cassert>
#include <new>

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#endif

#if defined(_WIN32) || defined(__MINGW32__)
# include "libdrizzle/windows.hpp"
# define get_socket_errno() WSAGetLastError()

#else
# include <netinet/tcp.h>
# include <sys/uio.h>
# include <unistd.h>
# include <cerrno>
# define INVALID_SOCKET -1
# define SOCKET_ERROR -1
# define closesocket(a) close(a)
# define get_socket_errno() errno

#endif // defined(_WIN32) || defined(__MINGW32__)

#if defined(HAVE_POLL_H) && HAVE_POLL_H
# include <poll.h>
typedef struct pollfd pollfd_t;
#else
# include "libdrizzle/poll.h"
#endif

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libdrizzle/structs.h"
#include "libdrizzle/drizzle_local.h"
#include "libdrizzle/conn_local.h"
#include "libdrizzle/pack.h"
#include "libdrizzle/state.h"
#include "libdrizzle/sha1.h"
#include "libdrizzle/statement_local.h"
#include "libdrizzle/column.h"
#include "libdrizzle/binlog.h"
#include "libdrizzle/handshake_client.h"
#include "libdrizzle/result.h"

#include <memory.h>
