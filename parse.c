#include "parse.h"
#include <stdlib.h>
#include "scan.h"

char* token_s(enum token_type t)
{
    if( t == HEADING_TOKEN ) return "HEADING";
    else if( t == CHARACTER_TOKEN ) return "CHARACTER";
    else if( t == PARAGRAPH_TOKEN ) return "PARAGRAPH";
    else if( t == INDENT_TOKEN ) return "INDENT";
    else if( t == END_TOKEN ) return "END";
    else return "DONNO";
}

void free_node(node* n)
{
    /* TODO: convert to loop */
    while( n->children != NULL ) free_node(n->children);
    while( n->siblings != NULL ) free_node(n->siblings);
    free(n);
}

node* parse_indent(FILE* is, lex_state* state)
{
    token* tok;
    tok = scan(is, state);
    printf("Got token %s\n", token_s(tok->type));
    return NULL;
}

node* parse_character(FILE* is, lex_state* state)
{
    token* tok = NULL;
    tok = scan(is, state);
    printf("Got '%c'\n", tok->ch);
    return NULL;
}

node* parse_indented_text(FILE* is, lex_state* state)
{
    node* child_node;
    token* tok;

    while( 1 ) {
        child_node = parse_character(is, state);
        if( tok->ch == '\n' ) {
            /* FIXME: maintain a buffer, like scanner's stream_buf_t */
            /* putt(tok, state); */
            break;
        }
    }
    return child_node;
}

node* parse_heading(FILE* is, lex_state* state)
{
    int i = 0;
    node* child_node;

    while( i < state->heading_level )
        free_node(parse_indent(is, state));
    child_node = parse_indented_text(is, state);
    return child_node;
}

node* parse_nl(FILE* is, lex_state* state)
{
    node* child_node;
    child_node = parse_character(is, state);
    /* TODO: check it's a newline */
    return NULL;
}

node* parse_paragraph(FILE* is, lex_state* state)
{
    node* child_node;
    child_node = parse_nl(is, state);
    child_node = parse_nl(is, state);
    do {
        child_node = parse_character(is, state);
    } while( child_node->type == CHARACTER_NODE );
    return child_node;
}

/* start */
node* parse_text(FILE* is, lex_state* state)
{
    token* tok = NULL;
    node *the_node = NULL, *child_node = NULL;

    the_node = (node*)malloc(sizeof(node));
    the_node->type = TEXT_NODE;
    the_node->children = (node*)malloc(sizeof(node));
    the_node->siblings = NULL;
    do {
        tok = scan(is, state);
        printf("Got token %s\n", token_s(tok->type));
        if( tok->type == HEADING_TOKEN ) {
            the_node->children->siblings = (node*)malloc(sizeof(node));
            child_node = parse_heading(is, state);
            the_node->children = child_node;
        } else if( tok->type == PARAGRAPH_TOKEN ) {
            child_node = parse_paragraph(is, state);
        } else if( tok->type == END_TOKEN ) { /* nothin */
        } else {
            fprintf(stderr, "%s:%d:Unexpected token %s\n",
                    state->filename, state->lineno, token_s(tok->type));
            free(the_node->children);
            the_node->children = NULL;
        }
        free(tok);
    } while( tok->type != END_TOKEN );
    return the_node;
}
