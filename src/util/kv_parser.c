#include "kv_parser.h"

kv_parser_t *kv_parser_new(neon_buff_t *buff, char delim, char comment, char escape) {
    kv_parser_t *state = malloc(sizeof(kv_parser_t));
    state->buff = buff;
    state->pos = 0;
    state->delim = delim;
    state->comment = comment;
    state->escape = escape;
    return state;
}


