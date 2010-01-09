#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>
#include "scan.h"

typedef struct node {
    enum {
        TEXT_NODE,
        HEADING_NODE,
        PARAGRAPH_NODE,
        CHARACTER_NODE,
        END_NODE
    } type;
    struct node* children;
    struct node* siblings;
    char ch;
} node;

typedef struct token_buf_t {
    token* tok;
    struct token_buf_t* next;
} token_buf_t;

typedef struct parse_state {
    token_buf_t* token_buf;
} parse_state;

node* parse_text(FILE* is, lex_state* state, parse_state* pstate);
void free_node(node* n);

#endif // PARSE_H_
