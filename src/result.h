/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2013 Drizzle Developer Group
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

/**
 * @ingroup drizzle_result
 */
struct drizzle_result_st
{
  drizzle_st *con;
  drizzle_result_st *next;
  drizzle_result_st *prev;
  drizzle_result_options_t options;

  char info[DRIZZLE_MAX_INFO_SIZE];
  uint16_t error_code;
  char sqlstate[DRIZZLE_MAX_SQLSTATE_SIZE + 1];
  uint64_t insert_id;
  uint16_t warning_count;
  uint64_t affected_rows;

  uint16_t column_count;
  uint16_t column_current;
  drizzle_column_st *column_list;
  drizzle_column_st *column;
  drizzle_column_st *column_buffer;

  uint64_t row_count;
  uint64_t row_current;

  uint16_t field_current;         /* field index */
  uint16_t field_current_read;    /* index of the field currently being read */
  uint64_t field_total;           /* total length of the field currently being read */
  uint64_t field_offset;          /* offset within field of most recently read field fragment (0 if first/only fragment) */
  uint32_t field_size;            /* size of most recently read field value or fragment of field value; max 2^24 */
  drizzle_field_t field;
  drizzle_field_t *field_buffer;
  size_t *field_buffer_sizes;

  size_t row_list_size;
  drizzle_row_t row;
  drizzle_row_t *row_list;
  size_t *field_sizes;
  size_t **field_sizes_list;
  drizzle_binlog_st *binlog_event;
  bool binlog_checksums;
  uint8_t **null_bitmap_list;
  uint8_t *null_bitmap;
  uint16_t null_bitmap_length;
  uint16_t null_bitcount;
  bool binary_rows;

  drizzle_result_st() :
    con(NULL),
    next(NULL),
    prev(NULL),
    options(DRIZZLE_RESULT_NONE),
    error_code(0),
    insert_id(0),
    warning_count(0),
    affected_rows(0),
    column_count(0),
    column_current(0),
    column_list(NULL),
    column(NULL),
    column_buffer(NULL),
    row_count(0),
    row_current(0),
    field_current(0),
    field_current_read(0),
    field_total(0),
    field_offset(0),
    field_size(0),
    field(NULL),
    field_buffer(NULL),
    row_list_size(0),
    row(NULL),
    row_list(NULL),
    field_sizes(NULL),
    field_sizes_list(NULL),
    binlog_event(NULL),
    binlog_checksums(false),
    null_bitmap_list(NULL),
    null_bitmap(NULL),
    null_bitmap_length(0),
    null_bitcount(0),
    binary_rows(false)
  {
    info[0]= '\0';
    sqlstate[0]= '\0';
  }

  bool push_state(drizzle_state_fn* func_)
  {
    if (con)
    {
      return con->push_state(func_);
    }

    return false;
  }

  bool has_state() const
  {
    if (con)
    {
      return con->has_state();
    }

    return false;
  }
};
