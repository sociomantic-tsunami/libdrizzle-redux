### Add missing documentation for API

Some existing functions in the **query API** was not part of the **rst** documentation.

`include/libdrizzle-redux/query.h`

- `ssize_t drizzle_escape_str(drizzle_st *con, char **to, const char *from, const size_t from_size, bool is_pattern)`

The **MySQL** manual lists 11 characters which should be escaped, including 2 which should only be escaped for a string used in a pattern-matching context (e.g. **LIKE** clause).

The function `drizzle_escape_str` extends the capabilities of `drizzle_escape_string` by taking an extra parameter, `is_pattern`, specifying whether the string will be used in a pattern-matching context.

The feature was part of [`v5.6.0`](https://github.com/sociomantic-tsunami/libdrizzle-redux/releases/tag/v5.6.0) but wasn't added to the documentation.
