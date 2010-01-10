#ifndef SCAN_H_
#define SCAN_H_

#include <stdio.h>
#include "token.h"

typedef struct stream_buf_t {
    int ch;
    struct stream_buf_t* next;
} stream_buf_t;

typedef struct token_buf_t {
    token* tok;
    struct token_buf_t* next;
} token_buf_t;

typedef struct lex_state {
    int beginning_of_line;
    int indenting;
    int paragraph_separator;
    int lineno;
    int heading_level;
    char* filename;
    enum token_type previous_token;
    stream_buf_t* stream_buf;
    token_buf_t* token_buf;
} lex_state;

token* scan(FILE* is, lex_state* state);
<<<<<<< Updated upstream
token* peek(FILE* is, lex_state* state);
=======
void putback(token* tok, lex_state* lstate);
token* sip(FILE* is, lex_state* lstate);
>>>>>>> Stashed changes

#endif // SCAN_H_
