#include "parse.h"
#include "scan.h"
#include <stdlib.h>
#include <assert.h>

/* TODO: get rid of this when done */
static char* token_s(enum token_type t)
{
    if( t == HEADING_TOKEN ) return "HEADING";
    else if( t == CHARACTER_TOKEN ) return "CHARACTER";
    else if( t == PARAGRAPH_TOKEN ) return "PARAGRAPH";
    else if( t == INDENT_TOKEN ) return "INDENT";
    else if( t == END_TOKEN ) return "END";
    else if( t == UNDEFINED_TOKEN ) return "UNDEFINED";
    else return "DONNO";
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
static node* parse_indent(FILE* is, lex_state* lstate)
{
    token* tok;
    tok = sip(is, lstate);
    return NULL;
}

/* FIXME: return the entire heading as a single string */
static node* parse_indented_text(FILE* is, lex_state* lstate)
{
    node *the_node, *child_node, *pos;
    token* tok;

    the_node = (node*)malloc(sizeof(node));
    the_node->children = the_node->siblings = NULL;
    pos = the_node;
    while( 1 ) {
        tok = sip(is, lstate);
        if( tok->type == PARAGRAPH_TOKEN ) { /* FIXME: END_TOKEN? */
            putback(tok, lstate);
            break;
        } else if( tok->type != CHARACTER_TOKEN ) {
            fprintf(stderr, "%s:%d:Unexpected token %s\n",
                    lstate->filename, lstate->lineno, token_s(tok->type));
            /* FIXME: handle error properly */
            free_node(the_node);
            return NULL;
        }
        if( tok->ch == '\n' ) {
            putback(tok, lstate);
            pos->siblings = child_node = NULL;
            break;
        } else {
            child_node = (node*)malloc(sizeof(node));
            child_node->type = CHARACTER_NODE;
            child_node->ch = tok->ch;
            child_node->children = child_node->siblings = NULL;
            pos->siblings = child_node;
            pos = pos->siblings;
        }
        free(tok);
    }
    /* we only filled-in the_node->siblings, so we have to free the_node and
     * return the list starting at the_node->siblings */
    child_node = the_node;
    the_node = the_node->siblings;
    free(child_node);
    return the_node;
}

static node* parse_heading(FILE* is, lex_state* lstate)
{
    int i = 0;
    node* the_node;

    for(i = 0; i < lstate->heading_level; ++i )
        free_node(parse_indent(is, lstate));
    the_node = (node*)malloc(sizeof(node));
    the_node->type = HEADING_NODE;
    the_node->heading_level = i; /* FIXME: token already holds level */
    the_node->ch = 0;
    the_node->siblings = NULL;
    the_node->children = parse_indented_text(is, lstate);
    return the_node;
}

/* TODO: implement */
static node* parse_paragraph(FILE* is, lex_state* lstate)
{
    node *the_node, *child_node, *pos;
    token* tok, *tmpt;

    tok = sip(is, lstate);
    assert(tok->type == CHARACTER_TOKEN);
    assert(tok->ch == '\n');
    free(tok);
    tok = sip(is, lstate);
    assert(tok->type == CHARACTER_TOKEN);
    assert(tok->ch == '\n');
    free(tok);
    while( 1 ) {
        tok = sip(is, lstate);
        if( tok->type != CHARACTER_TOKEN || tok->ch != '\n' ) {
            putback(tok, lstate);
            break;
        }
        free(tok);
    }
    /* FIXME: paragraph node as one string */
    the_node = (node*)malloc(sizeof(node));
    the_node->type = PARAGRAPH_NODE;
    the_node->ch = 0;
    the_node->siblings = NULL;
    the_node->children = (node*)malloc(sizeof(node));
    pos = the_node->children;
    while( 1 ) {
        /* FIXME: handle character parsing errors, cleanup */
        tok = sip(is, lstate);
        if( tok->type == CHARACTER_TOKEN && tok->ch == '\n' ) {
            /* FIXME: handle character parsing errors, parsing */
            tmpt = sip(is, lstate);
            if( tmpt->type == CHARACTER_TOKEN && tmpt->ch == '\n' ) {
                putback(tmpt, lstate);
                putback(tok, lstate);
                break;
            } else {
                putback(tmpt, lstate);
                child_node = (node*)malloc(sizeof(node));
                child_node->ch = tok->ch;
                child_node->children = child_node->siblings = NULL;
                child_node->type = CHARACTER_NODE;
                free(tok);
            }
        } else if( tok->type == CHARACTER_TOKEN && tok->ch != '\n' ) {
            child_node = (node*)malloc(sizeof(node));
            child_node->ch = tok->ch;
            child_node->children = child_node->siblings = NULL;
            child_node->type = CHARACTER_NODE;
            free(tok);
        } else  { /* tok->type != CHARACTER_TOKEN */
            putback(tok, lstate);
            break;
        }
        pos->siblings = child_node;
        pos = pos->siblings;
    }
    pos = the_node->children;
    the_node->children = pos->siblings;
    free(pos);
    return the_node;
}

/* start */
node* parse_text(FILE* is, lex_state* lstate)
{
    token* tok = NULL;
    node *the_node, *child_node, *pos;

    the_node = (node*)malloc(sizeof(node));
    the_node->type = TEXT_NODE;
    the_node->siblings = NULL;
    the_node->children = (node*)malloc(sizeof(node));
    pos = the_node->children;
    do {
        tok = sip(is, lstate);
        if( tok->type == HEADING_TOKEN ) {
            child_node = parse_heading(is, lstate);
        } else if( tok->type == PARAGRAPH_TOKEN ) {
            child_node = parse_paragraph(is, lstate);
        } else if( tok->type == END_TOKEN ) { /* nothin */
            child_node = (node*)malloc(sizeof(node));
            child_node->type = END_NODE;
            child_node->ch = 0;
            child_node->children = child_node->siblings = NULL;
        } else { /* FIXME: allow non-paragraph text without heading */
            fprintf(stderr, "%s:%d:Unexpected token %s\n",
                    lstate->filename, lstate->lineno, token_s(tok->type));
            free(tok);
            return the_node; /* FIXME: abort on unexpected token?! */
        }
        /* FIXME: if children is a long list then here we overwrite everything
         * after the head of the list. the following loop will do but is slow.
         * maybe add tail to node and maintain that? */
        pos->siblings = child_node;
        while( pos->siblings != NULL )
            pos = pos->siblings;
        free(tok);
    } while( tok->type != END_TOKEN );
    pos = the_node->children;
    the_node->children = pos->siblings;
    free(pos);
    return the_node;
}
