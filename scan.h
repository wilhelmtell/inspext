#ifndef SCAN_H_
#define SCAN_H_

#include <stdio.h>
#include "token.h"

typedef struct lex_state {
    int beginning_of_line;
    int lineno;
    int level;
} lex_state;

token* scan(FILE* is, lex_state* state);

#endif // SCAN_H_
