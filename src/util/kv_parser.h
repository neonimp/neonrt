#pragma once

#ifndef RT_KV_PARSE_H
#define RT_KV_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../neon_common.h"
#include "../containers/hashmap.h"

#define KV_PARSER_MAX_LINE_LENGTH 1024
#define KVP_ERROR_LINE_TOO_LONG 0
#define KVP_ERROR_UNEXPECTED_EOF 1
#define KVP_ERROR_UNEXPECTED_CHAR 2

typedef void (*kv_parser_error_handler_t)(size_t line, size_t col, uint32_t errno, const char *msg);

struct kv_parser {
    neon_buff_t *buff;
    size_t pos;
    char delim;
    char comment;
    char escape;
};

typedef struct kv_parser kv_parser_t;

/**
 * @brief Create a new kv_parser_t instance.
 * @param buff Buffer to parse.
 * @param delim Delimiter to use.
 * @param comment Comment character to use.
 * @param escape Escape character to use.
 * @return A pointer to an initialized kv_parser_t instance.
 */
extern kv_parser_t *kv_parser_new(neon_buff_t *buff, char delim, char comment, char escape);

/**
 * @brief Create a new kv_parser_t instance, with default values.
 * @param buff Buffer to parse.
 */
extern kv_parser_t *kv_parser_new_default(neon_buff_t *buff);

/**
 * @brief Destroy a kv_parser_t instance.
 * @param self Reference to the kv_parser_t instance.
 */
extern void kv_parser_free(kv_parser_t *state);

/**
 * @brief Check if the key is present in the buffer.
 * @param self Reference to the kv_parser_t instance.
 * @param key Key to check.
 * @return True if the key is present, false otherwise.
 */
extern bool kv_parser_has_key(kv_parser_t *state, neon_buff_t *key);

/**
 * @brief Get the value for a key.
 * @param self Reference to the kv_parser_t instance.
 * @param key Key to get the value for.
 * @return A pointer to the value, or NULL if the key is not present.
 */
extern neon_buff_t *kv_parser_get_value(kv_parser_t *state, neon_buff_t *key);

#ifdef __cplusplus
}
#endif

#endif // RT_KV_PARSE_H
