#include "parse.h"
#include <stdlib.h>
#include "scan.h"

static char* token_s(enum token_type t)
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
    if( n != NULL ) {
        while( n->children != NULL ) free_node(n->children);
        while( n->siblings != NULL ) free_node(n->siblings);
    }
    free(n);
}

static node* parse_indent(FILE* is, lex_state* lstate, parse_state* pstate)
{
    token* tok;
    tok = scan(is, lstate);
    printf("Got token %s\n", token_s(tok->type));
    return NULL;
}

static node* parse_character(FILE* is, lex_state* lstate, parse_state* pstate)
{
    node* child_node = NULL;
    token* tok = NULL;

    tok = scan(is, lstate);
    child_node = (node*)malloc(sizeof(node));
    child_node->ch = tok->ch;
    child_node->children = child_node->siblings = NULL;
    child_node->type = CHARACTER_NODE;
    if( tok->ch == '\n' )
        printf("Got NL\n");
    else
        printf("Got '%c'\n", tok->ch);
    return child_node;
}

static node* parse_indented_text(FILE* is, lex_state* lstate, parse_state* pstate)
{
    node* child_node;

    while( 1 ) {
        child_node = parse_character(is, lstate, pstate);
        if( child_node->ch == '\n' ) {
            /* FIXME: maintain a buffer, like scanner's stream_buf_t */
            /* putt(tok, lstate); */
            break;
        }
    }
    return child_node;
}

static node* parse_heading(FILE* is, lex_state* lstate, parse_state* pstate)
{
    int i = 0;
    node* child_node;

    for(i = 0; i < lstate->heading_level; ++i )
        free_node(parse_indent(is, lstate, pstate));
    child_node = parse_indented_text(is, lstate, pstate);
    return child_node;
}

static node* parse_nl(FILE* is, lex_state* lstate, parse_state* pstate)
{
    node* child_node;
    child_node = parse_character(is, lstate, pstate);
    /* TODO: check it's a newline */
    return NULL;
}

static node* parse_paragraph(FILE* is, lex_state* lstate, parse_state* pstate)
{
    node* child_node;
    child_node = parse_nl(is, lstate, pstate);
    child_node = parse_nl(is, lstate, pstate);
    do {
        child_node = parse_character(is, lstate, pstate);
    } while( child_node->type == CHARACTER_NODE );
    return child_node;
}

/* start */
node* parse_text(FILE* is, lex_state* lstate, parse_state* pstate)
{
    token* tok = NULL;
    node *the_node = NULL, *child_node = NULL;

    the_node = (node*)malloc(sizeof(node));
    the_node->type = TEXT_NODE;
    the_node->children = (node*)malloc(sizeof(node));
    the_node->siblings = NULL;
    do {
        tok = scan(is, lstate);
        printf("Got token %s\n", token_s(tok->type));
        if( tok->type == HEADING_TOKEN ) {
            the_node->children->siblings = (node*)malloc(sizeof(node));
            child_node = parse_heading(is, lstate, pstate);
            the_node->children = child_node;
        } else if( tok->type == PARAGRAPH_TOKEN ) {
            child_node = parse_paragraph(is, lstate, pstate);
        } else if( tok->type == END_TOKEN ) { /* nothin */
        } else {
            fprintf(stderr, "%s:%d:Unexpected token %s\n",
                    lstate->filename, lstate->lineno, token_s(tok->type));
            free_node(the_node->children);
            the_node->children = NULL;
        }
        free(tok);
    } while( tok->type != END_TOKEN );
    return the_node;
}
