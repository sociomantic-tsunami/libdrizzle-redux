/* vim:expandtab:shiftwidth=2:tabstop=2:smarttab:
 *
 * Drizzle Client & Protocol Library
 *
 * Copyright (C) 2008-2013 Drizzle Developer Group
 * Copyright (C) 2011 Brian Aker (brian@tangent.org)
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

#pragma once

/**
 * @file
 * @brief Defines, typedefs, enums, and macros
 */

#include <libdrizzle-5.1/return.h>
#include <libdrizzle-5.1/verbose.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup drizzle_constants Constants
 * @ingroup drizzle_client_interface
 * @ingroup drizzle_server_interface
 * @{
 */

/* Defines. */
#define DRIZZLE_DEFAULT_TCP_HOST         "localhost"
#define DRIZZLE_DEFAULT_TCP_PORT         3306
#define DRIZZLE_MYSQL_TCP_PORT           3306
#define DRIZZLE_MYSQL_TCP_SERVICE        "mysql"
#define DRIZZLE_DRIZZLE_TCP_PORT         4427
#define DRIZZLE_DEFAULT_TCP_SERVICE      "mysql"
#define DRIZZLE_DRIZZLE_TCP_SERVICE      "drizzle"
#define DRIZZLE_DEFAULT_UDS              "/tmp/mysql.sock"
#define DRIZZLE_DEFAULT_BACKLOG          64
#define DRIZZLE_MAX_ERROR_SIZE           2048
#define DRIZZLE_MAX_USER_SIZE            64
#define DRIZZLE_MAX_PASSWORD_SIZE        32
#define DRIZZLE_MAX_DB_SIZE              64
#define DRIZZLE_MAX_INFO_SIZE            2048
#define DRIZZLE_MAX_SQLSTATE_SIZE        5
#define DRIZZLE_MAX_CATALOG_SIZE         128
#define DRIZZLE_MAX_TABLE_SIZE           128
#define DRIZZLE_MAX_COLUMN_NAME_SIZE     2048
#define DRIZZLE_MAX_DEFAULT_VALUE_SIZE   2048
#define DRIZZLE_MAX_PACKET_SIZE          UINT32_MAX
#define DRIZZLE_MAX_BUFFER_SIZE          1024*1024*1024
#define DRIZZLE_DEFAULT_BUFFER_SIZE      1024*1024
#define DRIZZLE_BUFFER_COPY_THRESHOLD    8192
#define DRIZZLE_MAX_SERVER_VERSION_SIZE  32
#define DRIZZLE_MAX_SERVER_EXTRA_SIZE    32
#define DRIZZLE_MAX_SCRAMBLE_SIZE        20
#define DRIZZLE_STATE_STACK_SIZE         8
#define DRIZZLE_ROW_GROW_SIZE            8192
#define DRIZZLE_DEFAULT_SOCKET_TIMEOUT   10
#define DRIZZLE_DEFAULT_SOCKET_SEND_SIZE DRIZZLE_DEFAULT_BUFFER_SIZE
#define DRIZZLE_DEFAULT_SOCKET_RECV_SIZE DRIZZLE_DEFAULT_BUFFER_SIZE
#define DRIZZLE_MYSQL_PASSWORD_HASH      41
#define DRIZZLE_BINLOG_CRC32_LEN         4
// If this version or higher then we are doing checksums
#define DRIZZLE_BINLOG_CHECKSUM_VERSION  "5.6.1"

#define DRIZZLE_BINLOG_MAGIC             "\xFE\x62\x69\x6E"

/** @} */

#ifndef __cplusplus
typedef enum drizzle_socket_t drizzle_socket_t;
#endif

/**
 * @ingroup drizzle_con
 * Status flags for drizle_st.
 */
enum drizzle_status_t
{
  DRIZZLE_CON_STATUS_NONE=                     0,
  DRIZZLE_CON_STATUS_IN_TRANS=                 (1 << 0),
  DRIZZLE_CON_STATUS_AUTOCOMMIT=               (1 << 1),
  DRIZZLE_CON_STATUS_MORE_RESULTS_EXISTS=      (1 << 3),
  DRIZZLE_CON_STATUS_QUERY_NO_GOOD_INDEX_USED= (1 << 4),
  DRIZZLE_CON_STATUS_QUERY_NO_INDEX_USED=      (1 << 5),
  DRIZZLE_CON_STATUS_CURSOR_EXISTS=            (1 << 6),
  DRIZZLE_CON_STATUS_LAST_ROW_SENT=            (1 << 7),
  DRIZZLE_CON_STATUS_DB_DROPPED=               (1 << 8),
  DRIZZLE_CON_STATUS_NO_BACKSLASH_ESCAPES=     (1 << 9),
  DRIZZLE_CON_STATUS_QUERY_WAS_SLOW=           (1 << 10)
};

#ifndef __cplusplus
typedef enum drizzle_status_t drizzle_status_t;
#endif

/**
 * @ingroup drizzle_con
 * Capabilities for drizzle_st.
 */
enum drizzle_capabilities_t
{
  DRIZZLE_CAPABILITIES_NONE=                   0,
  DRIZZLE_CAPABILITIES_LONG_PASSWORD=          (1 << 0),
  DRIZZLE_CAPABILITIES_FOUND_ROWS=             (1 << 1),
  DRIZZLE_CAPABILITIES_LONG_FLAG=              (1 << 2),
  DRIZZLE_CAPABILITIES_CONNECT_WITH_DB=        (1 << 3),
  DRIZZLE_CAPABILITIES_NO_SCHEMA=              (1 << 4),
  DRIZZLE_CAPABILITIES_COMPRESS=               (1 << 5),
  DRIZZLE_CAPABILITIES_ODBC=                   (1 << 6),
  DRIZZLE_CAPABILITIES_LOCAL_FILES=            (1 << 7),
  DRIZZLE_CAPABILITIES_IGNORE_SPACE=           (1 << 8),
  DRIZZLE_CAPABILITIES_PROTOCOL_41=            (1 << 9),
  DRIZZLE_CAPABILITIES_INTERACTIVE=            (1 << 10),
  DRIZZLE_CAPABILITIES_SSL=                    (1 << 11),
  DRIZZLE_CAPABILITIES_IGNORE_SIGPIPE=         (1 << 12),
  DRIZZLE_CAPABILITIES_TRANSACTIONS=           (1 << 13),
  DRIZZLE_CAPABILITIES_RESERVED=               (1 << 14),
  DRIZZLE_CAPABILITIES_SECURE_CONNECTION=      (1 << 15),
  DRIZZLE_CAPABILITIES_MULTI_STATEMENTS=       (1 << 16),
  DRIZZLE_CAPABILITIES_MULTI_RESULTS=          (1 << 17),
  DRIZZLE_CAPABILITIES_PS_MULTI_RESULTS=       (1 << 18),
  DRIZZLE_CAPABILITIES_PLUGIN_AUTH=            (1 << 19),
  DRIZZLE_CAPABILITIES_SSL_VERIFY_SERVER_CERT= (1 << 30),
  DRIZZLE_CAPABILITIES_REMEMBER_OPTIONS=       (1 << 31),
  DRIZZLE_CAPABILITIES_CLIENT= (DRIZZLE_CAPABILITIES_LONG_PASSWORD |
                                DRIZZLE_CAPABILITIES_FOUND_ROWS |
                                DRIZZLE_CAPABILITIES_LONG_FLAG |
                                DRIZZLE_CAPABILITIES_CONNECT_WITH_DB |
                                DRIZZLE_CAPABILITIES_PLUGIN_AUTH |
                                DRIZZLE_CAPABILITIES_TRANSACTIONS |
                                DRIZZLE_CAPABILITIES_PROTOCOL_41 |
                                DRIZZLE_CAPABILITIES_SECURE_CONNECTION)
};

#ifndef __cplusplus
typedef enum drizzle_capabilities_t drizzle_capabilities_t;
#endif

/**
 * @ingroup drizzle_result
 * Options for drizzle_result_st.
 */
enum drizzle_result_options_t
{
  DRIZZLE_RESULT_NONE=          0,
  DRIZZLE_RESULT_SKIP_COLUMN=   (1 << 1),
  DRIZZLE_RESULT_BUFFER_COLUMN= (1 << 2),
  DRIZZLE_RESULT_BUFFER_ROW=    (1 << 3),
  DRIZZLE_RESULT_EOF_PACKET=    (1 << 4),
  DRIZZLE_RESULT_ROW_BREAK=     (1 << 5),
  DRIZZLE_RESULT_BINARY_ROWS=   (1 << 6)
};

#ifndef __cplusplus
typedef enum drizzle_result_options_t drizzle_result_options_t;
#endif

/**
 * @ingroup drizzle_column
 * Options for drizzle_column_st, currently unused.
 */
enum drizzle_column_options_t
{
  DRIZZLE_COLUMN_UNUSED= (1 << 0)
};

#ifndef __cplusplus
typedef enum drizzle_column_options_t drizzle_column_options_t;
#endif

/**
 * @ingroup drizzle_column
 * Types for drizzle_column_st.
 */
enum drizzle_column_type_t
{
  DRIZZLE_COLUMN_TYPE_DECIMAL,
  DRIZZLE_COLUMN_TYPE_TINY,
  DRIZZLE_COLUMN_TYPE_SHORT,
  DRIZZLE_COLUMN_TYPE_LONG,
  DRIZZLE_COLUMN_TYPE_FLOAT,
  DRIZZLE_COLUMN_TYPE_DOUBLE,
  DRIZZLE_COLUMN_TYPE_NULL,
  DRIZZLE_COLUMN_TYPE_TIMESTAMP,
  DRIZZLE_COLUMN_TYPE_LONGLONG,
  DRIZZLE_COLUMN_TYPE_INT24,
  DRIZZLE_COLUMN_TYPE_DATE,
  DRIZZLE_COLUMN_TYPE_TIME,
  DRIZZLE_COLUMN_TYPE_DATETIME,
  DRIZZLE_COLUMN_TYPE_YEAR,
  DRIZZLE_COLUMN_TYPE_NEWDATE,
  DRIZZLE_COLUMN_TYPE_VARCHAR,
  DRIZZLE_COLUMN_TYPE_BIT,
  DRIZZLE_COLUMN_TYPE_TIMESTAMP2,
  DRIZZLE_COLUMN_TYPE_DATETIME2,
  DRIZZLE_COLUMN_TYPE_TIME2,
  DRIZZLE_COLUMN_TYPE_NEWDECIMAL=  246,
  DRIZZLE_COLUMN_TYPE_ENUM=        247,
  DRIZZLE_COLUMN_TYPE_SET=         248,
  DRIZZLE_COLUMN_TYPE_TINY_BLOB=   249,
  DRIZZLE_COLUMN_TYPE_MEDIUM_BLOB= 250,
  DRIZZLE_COLUMN_TYPE_LONG_BLOB=   251,
  DRIZZLE_COLUMN_TYPE_BLOB=        252,
  DRIZZLE_COLUMN_TYPE_VAR_STRING=  253,
  DRIZZLE_COLUMN_TYPE_STRING=      254,
  DRIZZLE_COLUMN_TYPE_GEOMETRY=    255,
  DRIZZLE_COLUMN_TYPE_NONE= 0
};

#ifndef __cplusplus
typedef enum drizzle_column_type_t drizzle_column_type_t;
#endif

/**
 * @ingroup drizzle_column
 * Flags for drizzle_column_st.
 */
enum drizzle_column_flags_t
{
  DRIZZLE_COLUMN_FLAGS_NONE=             0,
  DRIZZLE_COLUMN_FLAGS_NOT_NULL=         (1 << 0),
  DRIZZLE_COLUMN_FLAGS_PRI_KEY=          (1 << 1),
  DRIZZLE_COLUMN_FLAGS_UNIQUE_KEY=       (1 << 2),
  DRIZZLE_COLUMN_FLAGS_MULTIPLE_KEY=     (1 << 3),
  DRIZZLE_COLUMN_FLAGS_BLOB=             (1 << 4),
  DRIZZLE_COLUMN_FLAGS_UNSIGNED=         (1 << 5),
  DRIZZLE_COLUMN_FLAGS_ZEROFILL=         (1 << 6),
  DRIZZLE_COLUMN_FLAGS_BINARY=           (1 << 7),
  DRIZZLE_COLUMN_FLAGS_ENUM=             (1 << 8),
  DRIZZLE_COLUMN_FLAGS_AUTO_INCREMENT=   (1 << 9),
  DRIZZLE_COLUMN_FLAGS_TIMESTAMP=        (1 << 10),
  DRIZZLE_COLUMN_FLAGS_SET=              (1 << 11),
  DRIZZLE_COLUMN_FLAGS_NO_DEFAULT_VALUE= (1 << 12),
  DRIZZLE_COLUMN_FLAGS_ON_UPDATE_NOW=    (1 << 13),
  DRIZZLE_COLUMN_FLAGS_PART_KEY=         (1 << 14),
  DRIZZLE_COLUMN_FLAGS_NUM=              (1 << 15),
  DRIZZLE_COLUMN_FLAGS_GROUP=            (1 << 15), /* NUM & GROUP the same. */
  DRIZZLE_COLUMN_FLAGS_UNIQUE=           (1 << 16),
  DRIZZLE_COLUMN_FLAGS_BINCMP=           (1 << 17),
  DRIZZLE_COLUMN_FLAGS_GET_FIXED_FIELDS= (1 << 18),
  DRIZZLE_COLUMN_FLAGS_IN_PART_FUNC=     (1 << 19),
  DRIZZLE_COLUMN_FLAGS_IN_ADD_INDEX=     (1 << 20),
  DRIZZLE_COLUMN_FLAGS_RENAMED=          (1 << 21)
};

typedef enum
{
  DRIZZLE_SSL_STATE_NONE= 0,
  DRIZZLE_SSL_STATE_HANDSHAKE_COMPLETE
} drizzle_ssl_state_t;

typedef enum
{
  DRIZZLE_EVENT_POSITION_TIMESTAMP= 0,
  DRIZZLE_EVENT_POSITION_TYPE= 4,
  DRIZZLE_EVENT_POSITION_SERVERID= 5,
  DRIZZLE_EVENT_POSITION_LENGTH= 9,
  DRIZZLE_EVENT_POSITION_NEXT= 13,
  DRIZZLE_EVENT_POSITION_FLAGS= 17,
  DRIZZLE_EVENT_POSITION_EXTRA_FLAGS= 19
} drizzle_binlog_event_positions_t;

typedef enum
{
  DRIZZLE_EVENT_TYPE_UNKNOWN= 0,
  DRIZZLE_EVENT_TYPE_START= 1,
  DRIZZLE_EVENT_TYPE_QUERY= 2,
  DRIZZLE_EVENT_TYPE_STOP= 3,
  DRIZZLE_EVENT_TYPE_ROTATE= 4,
  DRIZZLE_EVENT_TYPE_INTVAR= 5,
  DRIZZLE_EVENT_TYPE_LOAD= 6,
  DRIZZLE_EVENT_TYPE_SLAVE= 7,
  DRIZZLE_EVENT_TYPE_CREATE_FILE= 8,
  DRIZZLE_EVENT_TYPE_APPEND_BLOCK= 9,
  DRIZZLE_EVENT_TYPE_EXEC_LOAD= 10,
  DRIZZLE_EVENT_TYPE_DELETE_FILE= 11,
  DRIZZLE_EVENT_TYPE_NEW_LOAD= 12,
  DRIZZLE_EVENT_TYPE_RAND= 13,
  DRIZZLE_EVENT_TYPE_USER_VAR= 14,
  DRIZZLE_EVENT_TYPE_FORMAT_DESCRIPTION= 15,
  DRIZZLE_EVENT_TYPE_XID= 16,
  DRIZZLE_EVENT_TYPE_BEGIN_LOAD_QUERY= 17,
  DRIZZLE_EVENT_TYPE_EXECUTE_LOAD_QUERY= 18,
  DRIZZLE_EVENT_TYPE_TABLE_MAP= 19,
  // Next 3 are used in 5.1.0 -> 5.1.15 only
  DRIZZLE_EVENT_TYPE_OBSOLETE_WRITE_ROWS= 20,
  DRIZZLE_EVENT_TYPE_OBSOLETE_UPDATE_ROWS= 21,
  DRIZZLE_EVENT_TYPE_OBSOLETE_DELETE_ROWS= 22,
  DRIZZLE_EVENT_TYPE_V1_WRITE_ROWS= 23,
  DRIZZLE_EVENT_TYPE_V1_UPDATE_ROWS= 24,
  DRIZZLE_EVENT_TYPE_V1_DELETE_ROWS= 25,
  DRIZZLE_EVENT_TYPE_INCIDENT= 26,
  DRIZZLE_EVENT_TYPE_HEARTBEAT= 27,
  DRIZZLE_EVENT_TYPE_IGNORABLE= 28,
  DRIZZLE_EVENT_TYPE_ROWS_QUERY= 29,
  DRIZZLE_EVENT_TYPE_V2_WRITE_ROWS= 30,
  DRIZZLE_EVENT_TYPE_V2_UPDATE_ROWS= 31,
  DRIZZLE_EVENT_TYPE_V2_DELETE_ROWS= 32,
  DRIZZLE_EVENT_TYPE_GTID= 33,
  DRIZZLE_EVENT_TYPE_ANONYMOUS_GTID= 34,
  DRIZZLE_EVENT_TYPE_PREVIOUS_GTIDS= 35,
  DRIZZLE_EVENT_TYPE_END
} drizzle_binlog_event_types_t;

typedef enum
{
  DRIZZLE_CHARSET_NONE= 0,
  DRIZZLE_CHARSET_BIG5_CHINESE_CI= 1,
  DRIZZLE_CHARSET_LATIN2_CZECH_CS= 2,
  DRIZZLE_CHARSET_DEC8_SWEDISH_CI= 3,
  DRIZZLE_CHARSET_CP850_GENERAL_CI= 4,
  DRIZZLE_CHARSET_LATIN1_GERMAN1_CI= 5,
  DRIZZLE_CHARSET_HP8_ENGLISH_CI= 6,
  DRIZZLE_CHARSET_KOI8R_GENERAL_CI= 7,
  DRIZZLE_CHARSET_LATIN1_SWEDISH_CI= 8,
  DRIZZLE_CHARSET_LATIN2_GENERAL_CI= 9,
  DRIZZLE_CHARSET_SWE7_SWEDISH_CI= 10,
  DRIZZLE_CHARSET_ASCII_GENERAL_CI= 11,
  DRIZZLE_CHARSET_UJIS_JAPANESE_CI= 12,
  DRIZZLE_CHARSET_SJIS_JAPANESE_CI= 13,
  DRIZZLE_CHARSET_CP1251_BULGARIAN_CI= 14,
  DRIZZLE_CHARSET_LATIN1_DANISH_CI= 15,
  DRIZZLE_CHARSET_HEBREW_GENERAL_CI= 16,
  DRIZZLE_CHARSET_TIS620_THAI_CI= 18,
  DRIZZLE_CHARSET_EUCKR_KOREAN_CI= 19,
  DRIZZLE_CHARSET_LATIN7_ESTONIAN_CS= 20,
  DRIZZLE_CHARSET_LATIN2_HUNGARIAN_CI= 21,
  DRIZZLE_CHARSET_KOI8U_GENERAL_CI= 22,
  DRIZZLE_CHARSET_CP1251_UKRAINIAN_CI= 23,
  DRIZZLE_CHARSET_GB2312_CHINESE_CI= 24,
  DRIZZLE_CHARSET_GREEK_GENERAL_CI= 25,
  DRIZZLE_CHARSET_CP1250_GENERAL_CI= 26,
  DRIZZLE_CHARSET_LATIN2_CROATIAN_CI= 27,
  DRIZZLE_CHARSET_GBK_CHINESE_CI= 28,
  DRIZZLE_CHARSET_CP1257_LITHUANIAN_CI= 29,
  DRIZZLE_CHARSET_LATIN5_TURKISH_CI= 30,
  DRIZZLE_CHARSET_LATIN1_GERMAN2_CI= 31,
  DRIZZLE_CHARSET_ARMSCII8_GENERAL_CI= 32,
  DRIZZLE_CHARSET_UTF8_GENERAL_CI= 33,
  DRIZZLE_CHARSET_CP1250_CZECH_CS= 34,
  DRIZZLE_CHARSET_UCS2_GENERAL_CI= 35,
  DRIZZLE_CHARSET_CP866_GENERAL_CI= 36,
  DRIZZLE_CHARSET_KEYBCS2_GENERAL_CI= 37,
  DRIZZLE_CHARSET_MACCE_GENERAL_CI= 38,
  DRIZZLE_CHARSET_MACROMAN_GENERAL_CI= 39,
  DRIZZLE_CHARSET_CP852_GENERAL_CI= 40,
  DRIZZLE_CHARSET_LATIN7_GENERAL_CI= 41,
  DRIZZLE_CHARSET_LATIN7_GENERAL_CS= 42,
  DRIZZLE_CHARSET_MACCE_BIN= 43,
  DRIZZLE_CHARSET_CP1250_CROATIAN_CI= 44,
  DRIZZLE_CHARSET_UTF8MB4_GENERAL_CI= 45,
  DRIZZLE_CHARSET_UTF8MB4_BIN= 46,
  DRIZZLE_CHARSET_LATIN1_BIN= 47,
  DRIZZLE_CHARSET_LATIN1_GENERAL_CI= 48,
  DRIZZLE_CHARSET_LATIN1_GENERAL_CS= 49,
  DRIZZLE_CHARSET_CP1251_BIN= 50,
  DRIZZLE_CHARSET_CP1251_GENERAL_CI= 51,
  DRIZZLE_CHARSET_CP1251_GENERAL_CS= 52,
  DRIZZLE_CHARSET_MACROMAN_BIN= 53,
  DRIZZLE_CHARSET_UTF16_GENERAL_CI= 54,
  DRIZZLE_CHARSET_UTF16_BIN= 55,
  DRIZZLE_CHARSET_CP1256_GENERAL_CI= 57,
  DRIZZLE_CHARSET_CP1257_BIN= 58,
  DRIZZLE_CHARSET_CP1257_GENERAL_CI= 59,
  DRIZZLE_CHARSET_UTF32_GENERAL_CI= 60,
  DRIZZLE_CHARSET_UTF32_BIN= 61,
  DRIZZLE_CHARSET_BINARY= 63,
  DRIZZLE_CHARSET_ARMSCII8_BIN= 64,
  DRIZZLE_CHARSET_ASCII_BIN= 65,
  DRIZZLE_CHARSET_CP1250_BIN= 66,
  DRIZZLE_CHARSET_CP1256_BIN= 67,
  DRIZZLE_CHARSET_CP866_BIN= 68,
  DRIZZLE_CHARSET_DEC8_BIN= 69,
  DRIZZLE_CHARSET_GREEK_BIN= 70,
  DRIZZLE_CHARSET_HEBREW_BIN= 71,
  DRIZZLE_CHARSET_HP8_BIN= 72,
  DRIZZLE_CHARSET_KEYBCS2_BIN= 73,
  DRIZZLE_CHARSET_KOI8R_BIN= 74,
  DRIZZLE_CHARSET_KOI8U_BIN= 75,
  DRIZZLE_CHARSET_LATIN2_BIN= 77,
  DRIZZLE_CHARSET_LATIN5_BIN= 78,
  DRIZZLE_CHARSET_LATIN7_BIN= 79,
  DRIZZLE_CHARSET_CP850_BIN= 80,
  DRIZZLE_CHARSET_CP852_BIN= 81,
  DRIZZLE_CHARSET_SWE7_BIN= 82,
  DRIZZLE_CHARSET_UTF8_BIN= 83,
  DRIZZLE_CHARSET_BIG5_BIN= 84,
  DRIZZLE_CHARSET_EUCKR_BIN= 85,
  DRIZZLE_CHARSET_GB2312_BIN= 86,
  DRIZZLE_CHARSET_GBK_BIN= 87,
  DRIZZLE_CHARSET_SJIS_BIN= 88,
  DRIZZLE_CHARSET_TIS620_BIN= 89,
  DRIZZLE_CHARSET_UCS2_BIN= 90,
  DRIZZLE_CHARSET_UJIS_BIN= 91,
  DRIZZLE_CHARSET_GEOSTD8_GENERAL_CI= 92,
  DRIZZLE_CHARSET_GEOSTD8_BIN= 93,
  DRIZZLE_CHARSET_LATIN1_SPANISH_CI= 94,
  DRIZZLE_CHARSET_CP932_JAPANESE_CI= 95,
  DRIZZLE_CHARSET_CP932_BIN= 96,
  DRIZZLE_CHARSET_EUCJPMS_JAPANESE_CI= 97,
  DRIZZLE_CHARSET_EUCJPMS_BIN= 98,
  DRIZZLE_CHARSET_CP1250_POLISH_CI= 99,
  DRIZZLE_CHARSET_UTF16_UNICODE_CI= 101,
  DRIZZLE_CHARSET_UTF16_ICELANDIC_CI= 102,
  DRIZZLE_CHARSET_UTF16_LATVIAN_CI= 103,
  DRIZZLE_CHARSET_UTF16_ROMANIAN_CI= 104,
  DRIZZLE_CHARSET_UTF16_SLOVENIAN_CI= 105,
  DRIZZLE_CHARSET_UTF16_POLISH_CI= 106,
  DRIZZLE_CHARSET_UTF16_ESTONIAN_CI= 107,
  DRIZZLE_CHARSET_UTF16_SPANISH_CI= 108,
  DRIZZLE_CHARSET_UTF16_SWEDISH_CI= 109,
  DRIZZLE_CHARSET_UTF16_TURKISH_CI= 110,
  DRIZZLE_CHARSET_UTF16_CZECH_CI= 111,
  DRIZZLE_CHARSET_UTF16_DANISH_CI= 112,
  DRIZZLE_CHARSET_UTF16_LITHUANIAN_CI= 113,
  DRIZZLE_CHARSET_UTF16_SLOVAK_CI= 114,
  DRIZZLE_CHARSET_UTF16_SPANISH2_CI= 115,
  DRIZZLE_CHARSET_UTF16_ROMAN_CI= 116,
  DRIZZLE_CHARSET_UTF16_PERSIAN_CI= 117,
  DRIZZLE_CHARSET_UTF16_ESPERANTO_CI= 118,
  DRIZZLE_CHARSET_UTF16_HUNGARIAN_CI= 119,
  DRIZZLE_CHARSET_UTF16_SINHALA_CI= 120,
  DRIZZLE_CHARSET_UCS2_UNICODE_CI= 128,
  DRIZZLE_CHARSET_UCS2_ICELANDIC_CI= 129,
  DRIZZLE_CHARSET_UCS2_LATVIAN_CI= 130,
  DRIZZLE_CHARSET_UCS2_ROMANIAN_CI= 131,
  DRIZZLE_CHARSET_UCS2_SLOVENIAN_CI= 132,
  DRIZZLE_CHARSET_UCS2_POLISH_CI= 133,
  DRIZZLE_CHARSET_UCS2_ESTONIAN_CI= 134,
  DRIZZLE_CHARSET_UCS2_SPANISH_CI= 135,
  DRIZZLE_CHARSET_UCS2_SWEDISH_CI= 136,
  DRIZZLE_CHARSET_UCS2_TURKISH_CI= 137,
  DRIZZLE_CHARSET_UCS2_CZECH_CI= 138,
  DRIZZLE_CHARSET_UCS2_DANISH_CI= 139,
  DRIZZLE_CHARSET_UCS2_LITHUANIAN_CI= 140,
  DRIZZLE_CHARSET_UCS2_SLOVAK_CI= 141,
  DRIZZLE_CHARSET_UCS2_SPANISH2_CI= 142,
  DRIZZLE_CHARSET_UCS2_ROMAN_CI= 143,
  DRIZZLE_CHARSET_UCS2_PERSIAN_CI= 144,
  DRIZZLE_CHARSET_UCS2_ESPERANTO_CI= 145,
  DRIZZLE_CHARSET_UCS2_HUNGARIAN_CI= 146,
  DRIZZLE_CHARSET_UCS2_SINHALA_CI= 147,
  DRIZZLE_CHARSET_UCS2_GENERAL_MYSQL500_CI= 159,
  DRIZZLE_CHARSET_UTF32_UNICODE_CI= 160,
  DRIZZLE_CHARSET_UTF32_ICELANDIC_CI= 161,
  DRIZZLE_CHARSET_UTF32_LATVIAN_CI= 162,
  DRIZZLE_CHARSET_UTF32_ROMANIAN_CI= 163,
  DRIZZLE_CHARSET_UTF32_SLOVENIAN_CI= 164,
  DRIZZLE_CHARSET_UTF32_POLISH_CI= 165,
  DRIZZLE_CHARSET_UTF32_ESTONIAN_CI= 166,
  DRIZZLE_CHARSET_UTF32_SPANISH_CI= 167,
  DRIZZLE_CHARSET_UTF32_SWEDISH_CI= 168,
  DRIZZLE_CHARSET_UTF32_TURKISH_CI= 169,
  DRIZZLE_CHARSET_UTF32_CZECH_CI= 170,
  DRIZZLE_CHARSET_UTF32_DANISH_CI= 171,
  DRIZZLE_CHARSET_UTF32_LITHUANIAN_CI= 172,
  DRIZZLE_CHARSET_UTF32_SLOVAK_CI= 173,
  DRIZZLE_CHARSET_UTF32_SPANISH2_CI= 174,
  DRIZZLE_CHARSET_UTF32_ROMAN_CI= 175,
  DRIZZLE_CHARSET_UTF32_PERSIAN_CI= 176,
  DRIZZLE_CHARSET_UTF32_ESPERANTO_CI= 177,
  DRIZZLE_CHARSET_UTF32_HUNGARIAN_CI= 178,
  DRIZZLE_CHARSET_UTF32_SINHALA_CI= 179,
  DRIZZLE_CHARSET_UTF8_UNICODE_CI= 192,
  DRIZZLE_CHARSET_UTF8_ICELANDIC_CI= 193,
  DRIZZLE_CHARSET_UTF8_LATVIAN_CI= 194,
  DRIZZLE_CHARSET_UTF8_ROMANIAN_CI= 195,
  DRIZZLE_CHARSET_UTF8_SLOVENIAN_CI= 196,
  DRIZZLE_CHARSET_UTF8_POLISH_CI= 197,
  DRIZZLE_CHARSET_UTF8_ESTONIAN_CI= 198,
  DRIZZLE_CHARSET_UTF8_SPANISH_CI= 199,
  DRIZZLE_CHARSET_UTF8_SWEDISH_CI= 200,
  DRIZZLE_CHARSET_UTF8_TURKISH_CI= 201,
  DRIZZLE_CHARSET_UTF8_CZECH_CI= 202,
  DRIZZLE_CHARSET_UTF8_DANISH_CI= 203,
  DRIZZLE_CHARSET_UTF8_LITHUANIAN_CI= 204,
  DRIZZLE_CHARSET_UTF8_SLOVAK_CI= 205,
  DRIZZLE_CHARSET_UTF8_SPANISH2_CI= 206,
  DRIZZLE_CHARSET_UTF8_ROMAN_CI= 207,
  DRIZZLE_CHARSET_UTF8_PERSIAN_CI= 208,
  DRIZZLE_CHARSET_UTF8_ESPERANTO_CI= 209,
  DRIZZLE_CHARSET_UTF8_HUNGARIAN_CI= 210,
  DRIZZLE_CHARSET_UTF8_SINHALA_CI= 211,
  DRIZZLE_CHARSET_UTF8_GENERAL_MYSQL500_CI= 223,
  DRIZZLE_CHARSET_UTF8MB4_UNICODE_CI= 224,
  DRIZZLE_CHARSET_UTF8MB4_ICELANDIC_CI= 225,
  DRIZZLE_CHARSET_UTF8MB4_LATVIAN_CI= 226,
  DRIZZLE_CHARSET_UTF8MB4_ROMANIAN_CI= 227,
  DRIZZLE_CHARSET_UTF8MB4_SLOVENIAN_CI= 228,
  DRIZZLE_CHARSET_UTF8MB4_POLISH_CI= 229,
  DRIZZLE_CHARSET_UTF8MB4_ESTONIAN_CI= 230,
  DRIZZLE_CHARSET_UTF8MB4_SPANISH_CI= 231,
  DRIZZLE_CHARSET_UTF8MB4_SWEDISH_CI= 232,
  DRIZZLE_CHARSET_UTF8MB4_TURKISH_CI= 233,
  DRIZZLE_CHARSET_UTF8MB4_CZECH_CI= 234,
  DRIZZLE_CHARSET_UTF8MB4_DANISH_CI= 235,
  DRIZZLE_CHARSET_UTF8MB4_LITHUANIAN_CI= 236,
  DRIZZLE_CHARSET_UTF8MB4_SLOVAK_CI= 237,
  DRIZZLE_CHARSET_UTF8MB4_SPANISH2_CI= 238,
  DRIZZLE_CHARSET_UTF8MB4_ROMAN_CI= 239,
  DRIZZLE_CHARSET_UTF8MB4_PERSIAN_CI= 240,
  DRIZZLE_CHARSET_UTF8MB4_ESPERANTO_CI= 241,
  DRIZZLE_CHARSET_UTF8MB4_HUNGARIAN_CI= 242,
  DRIZZLE_CHARSET_UTF8MB4_SINHALA_CI= 243,
  DRIZZLE_CHARSET_MAX
} drizzle_charset_t;

typedef enum
{
  DRIZZLE_STMT_NONE= 0,
  DRIZZLE_STMT_PREPARED,
  DRIZZLE_STMT_EXECUTED,
  DRIZZLE_STMT_FETCHED
} drizzle_stmt_state_t;

#ifndef __cplusplus
typedef enum drizzle_column_flags_t drizzle_column_flags_t;
#endif

/**
 * @addtogroup drizzle_types Types
 * @ingroup drizzle_client_interface
 * @ingroup drizzle_server_interface
 * @{
 */

/* Types. */
typedef struct drizzle_tcp_st drizzle_tcp_st;
typedef struct drizzle_uds_st drizzle_uds_st;
typedef struct drizzle_st drizzle_st;
typedef struct drizzle_options_st drizzle_options_st;
typedef struct drizzle_result_st drizzle_result_st;
typedef struct drizzle_column_st drizzle_column_st;
typedef struct drizzle_binlog_st drizzle_binlog_st;
typedef struct drizzle_binlog_event_st drizzle_binlog_event_st;
typedef struct drizzle_stmt_st drizzle_stmt_st;
typedef struct drizzle_bind_st drizzle_bind_st;
typedef char *drizzle_field_t;
typedef drizzle_field_t *drizzle_row_t;

typedef void (drizzle_log_fn)(const char *line, drizzle_verbose_t verbose,
                              void *context);
typedef void (drizzle_binlog_fn)(drizzle_binlog_event_st *event, void *context);
typedef void (drizzle_binlog_error_fn)(drizzle_return_t error, drizzle_st *con, void *context);
typedef drizzle_return_t (drizzle_state_fn)(drizzle_st *con);
typedef void (drizzle_context_free_fn)(drizzle_st *con,
                                           void *context);
/**
 * Custom function to register or deregister interest in file descriptor
 * events. See drizzle_set_event_watch_fn().
 *
 * @param[in] con Connection that has changed the events it is interested in.
 *  Use drizzle_fd() to get the file descriptor.
 * @param[in] events A bit mask of POLLIN | POLLOUT, specifying if the
 *  connection is waiting for read or write events.
 * @param[in] context Application context pointer registered with
 *  drizzle_set_event_watch_fn().
 * @return DRIZZLE_RETURN_OK if successful.
 */
typedef drizzle_return_t (drizzle_event_watch_fn)(drizzle_st *con,
                                                  short events,
                                                  void *context);

/** @} */

/**
 * @addtogroup drizzle_macros Macros
 * @ingroup drizzle_client_interface
 * @ingroup drizzle_server_interface
 * @{
 */

/* Protocol unpacking macros. */
#define drizzle_get_byte2(__buffer)               \
            ((((uint8_t *)__buffer)[0]) |         \
  ((uint16_t)(((uint8_t *)__buffer)[1]) << 8))
#define drizzle_get_byte3(__buffer)               \
             (((uint8_t *)__buffer)[0] |          \
  ((uint32_t)(((uint8_t *)__buffer)[1]) << 8) |   \
  ((uint32_t)(((uint8_t *)__buffer)[2]) << 16))
#define drizzle_get_byte4(__buffer)               \
             (((uint8_t *)__buffer)[0] |          \
  ((uint32_t)(((uint8_t *)__buffer)[1]) << 8) |   \
  ((uint32_t)(((uint8_t *)__buffer)[2]) << 16) |  \
  ((uint32_t)(((uint8_t *)__buffer)[3]) << 24))
#define drizzle_get_byte8(__buffer)               \
  (drizzle_get_byte4(__buffer) |                  \
  ((uint64_t)drizzle_get_byte4(((uint8_t *)__buffer)+4) << 32))

/* Protocol packing macros. */
#define drizzle_set_byte1(__buffer, __int) do { \
    (__buffer)[0] = (uint8_t)(__int); } while (0)
#define drizzle_set_byte2(__buffer, __int) do { \
  (__buffer)[0]= (uint8_t)((__int) & 0xFF); \
  (__buffer)[1]= (uint8_t)(((__int) >> 8) & 0xFF); } while (0)
#define drizzle_set_byte3(__buffer, __int) do { \
  (__buffer)[0]= (uint8_t)((__int) & 0xFF); \
  (__buffer)[1]= (uint8_t)(((__int) >> 8) & 0xFF); \
  (__buffer)[2]= (uint8_t)(((__int) >> 16) & 0xFF); } while (0)
#define drizzle_set_byte4(__buffer, __int) do { \
  (__buffer)[0]= (uint8_t)((__int) & 0xFF); \
  (__buffer)[1]= (uint8_t)(((__int) >> 8) & 0xFF); \
  (__buffer)[2]= (uint8_t)(((__int) >> 16) & 0xFF); \
  (__buffer)[3]= (uint8_t)(((__int) >> 24) & 0xFF); } while (0)
#define drizzle_set_byte8(__buffer, __int) do { \
  (__buffer)[0]= (uint8_t)((__int) & 0xFF); \
  (__buffer)[1]= (uint8_t)(((__int) >> 8) & 0xFF); \
  (__buffer)[2]= (uint8_t)(((__int) >> 16) & 0xFF); \
  (__buffer)[3]= (uint8_t)(((__int) >> 24) & 0xFF); \
  (__buffer)[4]= (uint8_t)(((__int) >> 32) & 0xFF); \
  (__buffer)[5]= (uint8_t)(((__int) >> 40) & 0xFF); \
  (__buffer)[6]= (uint8_t)(((__int) >> 48) & 0xFF); \
  (__buffer)[7]= (uint8_t)(((__int) >> 56) & 0xFF); } while (0)

/* Multi-byte character macros. */
#define drizzle_mb_char(__c) (((__c) & 0x80) != 0)
#define drizzle_mb_length(__c) \
  ((uint32_t)(__c) <= 0x7f ? 1 : \
  ((uint32_t)(__c) <= 0x7ff ? 2 : \
  ((uint32_t)(__c) <= 0xd7ff ? 3 : \
  ((uint32_t)(__c) <= 0xdfff || (uint32_t)(__c) > 0x10ffff ? 0 : \
  ((uint32_t)(__c) <= 0xffff ? 3 : 4)))))

/** @} */

#ifdef __cplusplus
}
#endif
