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
    void* node_data;
} node;

node* parse_text(FILE* is, lex_state* state);

#endif // PARSE_H_
