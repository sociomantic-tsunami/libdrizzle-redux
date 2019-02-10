### Add const qualifier to params for non-mutating functions

- `include/libdrizzle-redux/binlog.h`

```diff

-uint32_t drizzle_binlog_event_timestamp(drizzle_binlog_event_st *event)
+uint32_t drizzle_binlog_event_timestamp(const drizzle_binlog_event_st *event)

-drizzle_binlog_event_types_t drizzle_binlog_event_type(drizzle_binlog_event_st *event)
+drizzle_binlog_event_types_t drizzle_binlog_event_type(const drizzle_binlog_event_st *event)

-uint32_t drizzle_binlog_event_server_id(drizzle_binlog_event_st *event)
+uint32_t drizzle_binlog_event_server_id(const drizzle_binlog_event_st *event)

-uint32_t drizzle_binlog_event_length(drizzle_binlog_event_st *event)
+uint32_t drizzle_binlog_event_length(const drizzle_binlog_event_st *event)

-uint32_t drizzle_binlog_event_next_pos(drizzle_binlog_event_st *event)
+uint32_t drizzle_binlog_event_next_pos(const drizzle_binlog_event_st *event)

-uint16_t drizzle_binlog_event_flags(drizzle_binlog_event_st *event)
+uint16_t drizzle_binlog_event_flags(const drizzle_binlog_event_st *event)

-uint16_t drizzle_binlog_event_extra_flags(drizzle_binlog_event_st *event)
+uint16_t drizzle_binlog_event_extra_flags(const drizzle_binlog_event_st *event)

-const unsigned char *drizzle_binlog_event_data(drizzle_binlog_event_st *event)
+const unsigned char *drizzle_binlog_event_data(const drizzle_binlog_event_st *event)

-const unsigned char *drizzle_binlog_event_raw_data(drizzle_binlog_event_st *event)
+const unsigned char *drizzle_binlog_event_raw_data(const drizzle_binlog_event_st *event)

-uint32_t drizzle_binlog_event_raw_length(drizzle_binlog_event_st *event)
+uint32_t drizzle_binlog_event_raw_length(const drizzle_binlog_event_st *event)

-const char *drizzle_binlog_event_type_str(drizzle_binlog_event_types_t event_type)
+const char *drizzle_binlog_event_type_str(const drizzle_binlog_event_types_t event_type)

```

- `include/libdrizzle-redux/column.h`

```diff

-const char *drizzle_column_catalog(drizzle_column_st *column)
+const char *drizzle_column_catalog(const drizzle_column_st *column)

-const char *drizzle_column_db(drizzle_column_st *column)
+const char *drizzle_column_db(const drizzle_column_st *column)

-const char *drizzle_column_table(drizzle_column_st *column)
+const char *drizzle_column_table(const drizzle_column_st *column)

-const char *drizzle_column_orig_table(drizzle_column_st *column)
+const char *drizzle_column_orig_table(const drizzle_column_st *column)

-const char *drizzle_column_name(drizzle_column_st *column)
+const char *drizzle_column_name(const drizzle_column_st *column)

-const char *drizzle_column_orig_name(drizzle_column_st *column)
+const char *drizzle_column_orig_name(const drizzle_column_st *column)

-drizzle_charset_t drizzle_column_charset(drizzle_column_st *column)
+drizzle_charset_t drizzle_column_charset(const drizzle_column_st *column)

-uint32_t drizzle_column_size(drizzle_column_st *column)
+uint32_t drizzle_column_size(const drizzle_column_st *column)

-size_t drizzle_column_max_size(drizzle_column_st *column)
+size_t drizzle_column_max_size(const drizzle_column_st *column)

-drizzle_column_type_t drizzle_column_type(drizzle_column_st *column)
+drizzle_column_type_t drizzle_column_type(const drizzle_column_st *column)

-const char *drizzle_column_type_str(drizzle_column_type_t type)
+const char *drizzle_column_type_str(const drizzle_column_type_t type)

-drizzle_column_flags_t drizzle_column_flags(drizzle_column_st *column)
+drizzle_column_flags_t drizzle_column_flags(const drizzle_column_st *column)

-uint8_t drizzle_column_decimals(drizzle_column_st *column)
+uint8_t drizzle_column_decimals(const drizzle_column_st *column)

-const unsigned char *drizzle_column_default_value(drizzle_column_st *column, size_t *size
+const unsigned char *drizzle_column_default_value(const drizzle_column_st *column, size_t *size)

```
- `include/libdrizzle-redux/column_client.h`

```diff

-drizzle_column_st *drizzle_column_index(drizzle_result_st *result, uint16_t column)
+drizzle_column_st *drizzle_column_index(const drizzle_result_st *result, uint16_t column)

-uint16_t drizzle_column_current(drizzle_result_st *result)
+uint16_t drizzle_column_current(const drizzle_result_st *result)
```

- `include/libdrizzle-redux/conn.h`

```diff
-int drizzle_socket_get_option(drizzle_st *con, drizzle_socket_option_t option)
+int drizzle_socket_get_option(const drizzle_st *con, drizzle_socket_option_t option)

-bool drizzle_options_get_non_blocking(drizzle_options_st *options)
+bool drizzle_options_get_non_blocking(const drizzle_options_st *options)

-bool drizzle_options_get_raw_scramble(drizzle_options_st *options)
+bool drizzle_options_get_raw_scramble(const drizzle_options_st *options)

-bool drizzle_options_get_found_rows(drizzle_options_st *options)
+bool drizzle_options_get_found_rows(const drizzle_options_st *options)

-bool drizzle_options_get_interactive(drizzle_options_st *options)
+bool drizzle_options_get_interactive(const drizzle_options_st *options)

-bool drizzle_options_get_multi_statements(drizzle_options_st *options)
+bool drizzle_options_get_multi_statements(const drizzle_options_st *options)

-bool drizzle_options_get_auth_plugin(drizzle_options_st *options)
+bool drizzle_options_get_auth_plugin(const drizzle_options_st *options)

-drizzle_socket_owner_t drizzle_options_get_socket_owner(drizzle_options_st *options)
+drizzle_socket_owner_t drizzle_options_get_socket_owner(const drizzle_options_st *options)

```

- `include/libdrizzle-redux/drizzle.h`

```diff
-const char *drizzle_verbose_name(drizzle_verbose_t verbose)
+const char *drizzle_verbose_name(const drizzle_verbose_t verbose)
```

- `include/libdrizzle-redux/query.h`
```diff

-ssize_t drizzle_escape_string(drizzle_st *con, char **to, const char *from, const size_t from_size)
+ssize_t drizzle_escape_string(const drizzle_st *con, char **to, const char *from, const size_t from_size)

-ssize_t drizzle_escape_str(drizzle_st *con, char **to, const char *from, const size_t from_size, bool is_pattern)
+ssize_t drizzle_escape_str(const drizzle_st *con, char **to, const char *from, const size_t from_size, bool is_pattern)
```

- `include/libdrizzle-redux/result.h`

```diff
-bool drizzle_result_eof(drizzle_result_st *result)
+bool drizzle_result_eof(const drizzle_result_st *result)

-const char *drizzle_result_message(drizzle_result_st *result)
+const char *drizzle_result_message(const drizzle_result_st *result)

-uint16_t drizzle_result_error_code(drizzle_result_st *result)
+uint16_t drizzle_result_error_code(const drizzle_result_st *result)

-const char *drizzle_result_sqlstate(drizzle_result_st *result)
+const char *drizzle_result_sqlstate(const drizzle_result_st *result)

-uint16_t drizzle_result_warning_count(drizzle_result_st *result)
+uint16_t drizzle_result_warning_count(const drizzle_result_st *result)

-uint64_t drizzle_result_insert_id(drizzle_result_st *result)
+uint64_t drizzle_result_insert_id(const drizzle_result_st *result)

-uint64_t drizzle_result_affected_rows(drizzle_result_st *result)
+uint64_t drizzle_result_affected_rows(const drizzle_result_st *result)

-uint16_t drizzle_result_column_count(drizzle_result_st *result)
+uint16_t drizzle_result_column_count(const drizzle_result_st *result)

-uint64_t drizzle_result_row_count(drizzle_result_st *result)
+uint64_t drizzle_result_row_count(const drizzle_result_st *result)
```

- `include/libdrizzle-redux/result_client.h`

```diff
-size_t drizzle_result_row_size(drizzle_result_st *result)
+size_t drizzle_result_row_size(const drizzle_result_st *result)
```

- `include/libdrizzle-redux/return.h`

```diff
-static inline bool drizzle_success(drizzle_return_t ret)
+static inline bool drizzle_success(const drizzle_return_t ret)
 {
-static inline bool drizzle_failed(drizzle_return_t ret)
+static inline bool drizzle_failed(const drizzle_return_t ret)
 {
```

- `include/libdrizzle-redux/row_client.h`

```diff
-size_t *drizzle_row_field_sizes(drizzle_result_st *result)
+size_t *drizzle_row_field_sizes(const drizzle_result_st *result)

-drizzle_row_t drizzle_row_index(drizzle_result_st *result, uint64_t row)
+drizzle_row_t drizzle_row_index(const drizzle_result_st *result, uint64_t row)

-uint64_t drizzle_row_current(drizzle_result_st *result)
+uint64_t drizzle_row_current(const drizzle_result_st *result)
```

- `include/libdrizzle-redux/statement.h`

```diff
-uint16_t drizzle_stmt_column_count(drizzle_stmt_st *stmt)
+uint16_t drizzle_stmt_column_count(const drizzle_stmt_st *stmt)

-uint64_t drizzle_stmt_affected_rows(drizzle_stmt_st *stmt)
+uint64_t drizzle_stmt_affected_rows(const drizzle_stmt_st *stmt)

-uint64_t drizzle_stmt_insert_id(drizzle_stmt_st *stmt)
+uint64_t drizzle_stmt_insert_id(const drizzle_stmt_st *stmt)

-uint16_t drizzle_stmt_param_count(drizzle_stmt_st *stmt)
+uint16_t drizzle_stmt_param_count(const drizzle_stmt_st *stmt)

-uint64_t drizzle_stmt_row_count(drizzle_stmt_st *stmt)
+uint64_t drizzle_stmt_row_count(const drizzle_stmt_st *stmt)

-bool drizzle_stmt_get_is_null_from_name(const drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
+bool drizzle_stmt_get_is_null_from_name(const drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

-bool drizzle_stmt_get_is_null(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
+bool drizzle_stmt_get_is_null(const drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

-bool drizzle_stmt_get_is_unsigned_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
+bool drizzle_stmt_get_is_unsigned_from_name(const drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

-bool drizzle_stmt_get_is_unsigned(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
+bool drizzle_stmt_get_is_unsigned(const drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

-const char *drizzle_stmt_get_string_from_name(drizzle_stmt_st *stmt, const char *column_name, size_t *len, drizzle_return_t *ret_ptr)
+const char *drizzle_stmt_get_string_from_name(const drizzle_stmt_st *stmt, const char *column_name, size_t *len, drizzle_return_t *ret_ptr)

-const char *drizzle_stmt_get_string(drizzle_stmt_st *stmt, uint16_t column_number, size_t *len, drizzle_return_t *ret_ptr)
+const char *drizzle_stmt_get_string(const drizzle_stmt_st *stmt, uint16_t column_number, size_t *len, drizzle_return_t *ret_ptr)

-uint32_t drizzle_stmt_get_int(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
+uint32_t drizzle_stmt_get_int(const drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

-uint32_t drizzle_stmt_get_int_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
+uint32_t drizzle_stmt_get_int_from_name(const drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

-uint64_t drizzle_stmt_get_bigint_from_name(drizzle_stmt_st *stmt, const char *column_name,drizzle_return_t *ret_ptr)
+uint64_t drizzle_stmt_get_bigint_from_name(const drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)

-uint64_t drizzle_stmt_get_bigint(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
+uint64_t drizzle_stmt_get_bigint(const drizzle_stmt_st *stmt, uint16_t column_number,drizzle_return_t *ret_ptr)

-double drizzle_stmt_get_double(drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)
+double drizzle_stmt_get_double(const drizzle_stmt_st *stmt, uint16_t column_number, drizzle_return_t *ret_ptr)

-double drizzle_stmt_get_double_from_name(drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
+double drizzle_stmt_get_double_from_name(const drizzle_stmt_st *stmt, const char *column_name, drizzle_return_t *ret_ptr)
```

### Add const qualifier to binlog callback function signature

- `include/libdrizzle-redux/constants.h`

```diff
-typedef void (drizzle_binlog_fn)(drizzle_binlog_event_st *event, void *context)
+typedef void (drizzle_binlog_fn)(const drizzle_binlog_event_st *event, void *context)
```
