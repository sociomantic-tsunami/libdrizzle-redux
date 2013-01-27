/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2012-2013 Drizzle Developer Group
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

#pragma once

#define LIBDRIZZLE_LIST_ADD(__list, __obj) { \
  if (__list ## _list != NULL) \
    __list ## _list->prev= __obj; \
  __obj->next= __list ## _list; \
  __obj->prev= NULL; \
  __list ## _list= __obj; \
  __list ## _count++; \
}

#define LIBDRIZZLE_LIST_DEL(__list, __obj) { \
  if (__list ## _list == __obj) \
    __list ## _list= __obj->next; \
  if (__obj->prev != NULL) \
    __obj->prev->next= __obj->next; \
  if (__obj->next != NULL) \
    __obj->next->prev= __obj->prev; \
  __list ## _count--; \
}

class Packet {
public:
  Packet():
    _stack(true),
    _drizzle(NULL),
    _func(NULL),
    next(NULL),
    prev(NULL)
  {
  }

  Packet(drizzle_st* drizzle_, drizzle_state_fn *func_):
    _stack(false),
    _drizzle(drizzle_),
    _func(func_),
    next(NULL),
    prev(NULL)
  {
  }

  ~Packet()
  {
  }

  void init(drizzle_st* drizzle_)
  {
    _drizzle= drizzle_;
  }

  void clear()
  {
    _func= NULL;
  }

  bool stack() const
  {
    return _stack;
  }

  void func(drizzle_state_fn* func_)
  {
    _func= func_;
  }

  drizzle_return_t func()
  {
    assert(_drizzle);
    if (_func)
    {
      return _func(_drizzle);
    }

    return DRIZZLE_RETURN_INVALID_ARGUMENT;
  }

public:
  const bool _stack;
  drizzle_st* _drizzle;
  drizzle_state_fn *_func;
  Packet *next;
  Packet *prev;
};
