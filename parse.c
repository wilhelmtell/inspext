#include "parse.h"
#include "scan.h"
#include <stdlib.h>

/* TODO: get rid of this when done */
static char* token_s(enum token_type t)
{
    if( t == HEADING_TOKEN ) return "HEADING";
    else if( t == CHARACTER_TOKEN ) return "CHARACTER";
    else if( t == PARAGRAPH_TOKEN ) return "PARAGRAPH";
    else if( t == INDENT_TOKEN ) return "INDENT";
    else if( t == END_TOKEN ) return "END";
    else return "DONNO";
}

static void putback(token* tok, parse_state* pstate)
{
    token_buf_t* tmp;

    tmp = pstate->token_buf;
    pstate->token_buf = (token_buf_t*)malloc(sizeof(token_buf_t));
    pstate->token_buf->tok = tok;
    pstate->token_buf->next = tmp;
    printf("Put back token %s", token_s(tok->type));
    if( tok->type == CHARACTER_TOKEN ) {
        if( tok->ch == '\n' ) printf(" NL");
        else printf(" '%c'", tok->ch);
    }
    printf("\n");
}

static token* sip(FILE* is, lex_state* lstate, parse_state* pstate)
{
    token_buf_t* tmp;
    token* tok;

    /* FIXME: report error when NULL */
    if( pstate->token_buf != NULL ) {
        tmp = pstate->token_buf;
        tok = pstate->token_buf->tok;
        pstate->token_buf = pstate->token_buf->next;
        free(tmp);
    } else {
        tok = scan(is, lstate);
    }
    printf("Got token %s", token_s(tok->type));
    if( tok->type == CHARACTER_TOKEN ) {
        if( tok->ch == '\n' ) printf(" NL");
        else printf(" '%c'", tok->ch);
    }
    printf("\n");
    return tok;
}

void free_node(node* n)
{
    node *p, *tmpc, *q, *tmps;

    if( n == NULL )
        return;
    for( tmpc = p = n->children; p != NULL; p = tmpc ) {
        for( tmps = q = p->siblings; q != NULL; q = tmps ) {
            tmps = q->siblings;
            free(q);
        }
        tmpc = p->children;
        free(p);
    }
    free(n);
}

/* TODO: implement */
static node* parse_indent(FILE* is, lex_state* lstate, parse_state* pstate)
{
    token* tok;
    tok = sip(is, lstate, pstate);
    return NULL;
}

/* FIXME: return the entire heading as a single string */
static node* parse_indented_text(FILE* is, lex_state* lstate, parse_state* pstate)
{
    node *the_node, *child_node, *pos;
    token* tok;

    the_node = (node*)malloc(sizeof(node));
    pos = the_node;
    while( 1 ) {
        tok = sip(is, lstate, pstate);
        if( tok->type == PARAGRAPH_TOKEN ) {
            putback(tok, pstate);
            break;
        } else if( tok->type != CHARACTER_TOKEN ) {
            fprintf(stderr, "ERROR:Unexpected token %s\n", token_s(tok->type));
            /* FIXME: handle error properly */
            free_node(the_node);
            return NULL;
        }
        child_node = (node*)malloc(sizeof(node));
        child_node->ch = tok->ch;
        child_node->children = child_node->siblings = NULL;
        child_node->type = CHARACTER_NODE;
        if( child_node->ch == '\n' ) {
            tok = (token*)malloc(sizeof(token));
            tok->type = CHARACTER_TOKEN;
            tok->ch = '\n';
            putback(tok, pstate);
            free_node(child_node);
            pos->siblings = child_node = NULL;
            break;
        } else {
            pos->siblings = child_node;
            pos = pos->siblings;
        }
    }
    return the_node;
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

/* TODO: get rid of this fn.  NL is a terminal */
static node* parse_nl(FILE* is, lex_state* lstate, parse_state* pstate)
{
    node* the_node;
    token* tok;

    /* FIXME: handle character parsing errors, cleanup */
    tok = sip(is, lstate, pstate);
    the_node = (node*)malloc(sizeof(node));
    the_node->ch = tok->ch;
    the_node->children = the_node->siblings = NULL;
    the_node->type = CHARACTER_NODE;
    if( the_node->ch != '\n' ) {
        tok = (token*)malloc(sizeof(token));
        tok->type = CHARACTER_TOKEN;
        tok->ch = the_node->ch;
        fprintf(stderr, "ERROR:Expected newline, found '%c'\n", the_node->ch);
        putback(tok, pstate);
        return NULL;
    }
    return the_node;
}

/* TODO: implement */
static node* parse_paragraph(FILE* is, lex_state* lstate, parse_state* pstate)
{
    node *the_node, *child_node, *pos, *tmp;
    token* tok;

    child_node = parse_nl(is, lstate, pstate);
    if( child_node == NULL )
        return NULL;
    else
        free_node(child_node);
    child_node = parse_nl(is, lstate, pstate);
    if( child_node == NULL )
        return NULL;
    else
        free_node(child_node);
    while( 1 ) {
        /* FIXME: handle character parsing errors, cleanup */
        tok = sip(is, lstate, pstate);
        child_node = (node*)malloc(sizeof(node));
        child_node->ch = tok->ch;
        child_node->children = child_node->siblings = NULL;
        child_node->type = CHARACTER_NODE;
        if( child_node == NULL )
            return NULL;
        else if( child_node->ch != '\n' ) {
            tok = (token*)malloc(sizeof(token));
            tok->type = CHARACTER_TOKEN;
            tok->ch = child_node->ch;
            putback(tok, pstate);
            free_node(child_node);
            break;
        }
        free_node(child_node);
    }
    /* FIXME: paragraph node as one string */
    the_node = (node*)malloc(sizeof(node));
    the_node->siblings = NULL;
    the_node->children = (node*)malloc(sizeof(node*));
    pos = the_node->children;
    pos->children = pos->siblings = NULL;
    while( 1 ) {
        /* FIXME: handle character parsing errors, cleanup */
        tok = sip(is, lstate, pstate);
        child_node = (node*)malloc(sizeof(node));
        child_node->ch = tok->ch;
        child_node->children = child_node->siblings = NULL;
        child_node->type = CHARACTER_NODE;
        if( child_node == NULL ) {
            free_node(the_node);
            return NULL;
        }
        else if( child_node->type == END_NODE ) {
            tok = (token*)malloc(sizeof(token));
            tok->type = END_TOKEN;
            putback(tok, pstate);
            break;
        }
        else if( child_node->type == CHARACTER_NODE && child_node->ch == '\n' ) {
            tmp = child_node;
            /* FIXME: handle character parsing errors, parsing */
            tok = sip(is, lstate, pstate);
            child_node = (node*)malloc(sizeof(node));
            child_node->ch = tok->ch;
            child_node->children = child_node->siblings = NULL;
            child_node->type = CHARACTER_NODE;
            if( child_node->type == CHARACTER_NODE && child_node->ch == '\n' ) {
                tok = (token*)malloc(sizeof(token));
                tok->type = CHARACTER_TOKEN;
                tok->ch = '\n';
                putback(tok, pstate);
                tok = (token*)malloc(sizeof(token));
                tok->type = CHARACTER_TOKEN;
                tok->ch = '\n';
                putback(tok, pstate);
                free_node(child_node);
                free_node(tmp);
                break;
            } else {
                tok = (token*)malloc(sizeof(token));
                tok->type = CHARACTER_TOKEN;
                tok->ch = '\n';
                putback(tok, pstate);
                free_node(child_node);
            }
            child_node = tmp;
            pos->children = child_node;
            pos = pos->children;
        }
    }
    return the_node;
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
        tok = sip(is, lstate, pstate);
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
