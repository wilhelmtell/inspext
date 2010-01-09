#ifndef SCAN_H_
#define SCAN_H_

#include <stdio.h>
#include "token.h"

typedef struct stream_buf_t {
    int ch;
    struct stream_buf_t* next;
} stream_buf_t;

typedef struct lex_state {
    int beginning_of_line;
    int indenting;
    int paragraph_separator;
    int lineno;
    int heading_level;
    char* filename;
    enum token_type previous_token;
    stream_buf_t* stream_buf;
} lex_state;

token* scan(FILE* is, lex_state* state);
token* peek(FILE* is, lex_state* state);

#endif // SCAN_H_
