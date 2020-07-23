### Extend SSL client API

Add client API functions to get the cipher name and protocol version of the established connection

`include/libdrizzle-redux/ssl.h`

- `drizzle_return_t drizzle_ssl_get_cipher_name(const drizzle_st * con, const char **cipher_name)`


- `drizzle_return_t drizzle_ssl_cipher_get_version(const drizzle_st *con, const char **cipher_version)`




