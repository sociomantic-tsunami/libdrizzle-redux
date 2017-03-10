/* LibMemcached
 * Copyright (C) 2012-2013 Drizzle Developer Group
 * Copyright (C) 2010 Brian Aker, Trond Norbye
 * All rights reserved.
 *
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in the parent directory for full text.
 *
 * Summary: Implementation of poll by using select
 *
 */

#pragma once

#if defined(WIN32) || defined(__MINGW32__)

#include <winsock2.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pollfd
{
#ifdef WIN32
  SOCKET fd;
#else
  int fd;
#endif
  short events;
  short revents;
} pollfd_t;

typedef int nfds_t;

#define POLLIN 0x0001
#define POLLOUT 0x0004
#define POLLERR 0x0008
#define POLLHUP		0x010		/* Hung up.  */
#define POLLNVAL	0x020		/* Invalid polling request.  */

int poll(struct pollfd fds[], nfds_t nfds, int tmo);

#ifdef __cplusplus
}
#endif

#endif // defined(WIN32) || defined(__MINGW32__)
