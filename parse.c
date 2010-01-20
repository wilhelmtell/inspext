/******************************************************************************
 * Copyright (C) 2010 Matan Nassau
 *
 * This file is part of INSPext.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#include "parse.h"
#include "scan.h"
#include <stdlib.h>
#include <assert.h>

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

static node* parse_indent(FILE* is, lex_state* lstate)
{
    token* tok;
    node* the_node;

    tok = scan(is, lstate);
    the_node = (node*)malloc(sizeof(node));
    the_node->ch = 0;
    the_node->heading_level = 0;
    the_node->type = INDENT_NODE;
    the_node->children = the_node->siblings = NULL;
    free(tok);
    return the_node;
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
        tok = scan(is, lstate);
        if( tok->type == PARAGRAPH_TOKEN ||
                tok->type == HEADING_TOKEN ||
                tok->type == END_TOKEN) {
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

static node* parse_paragraph(FILE* is, lex_state* lstate)
{
    node *the_node, *child_node, *pos;
    token* tok, *tmpt;

    while( 1 ) {
        tok = scan(is, lstate);
        assert(tok->type == CHARACTER_TOKEN);
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
        tok = scan(is, lstate);
        if( tok->type == CHARACTER_TOKEN && tok->ch == '\n' ) {
            /* FIXME: handle character parsing errors, parsing */
            tmpt = scan(is, lstate);
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
        free(tok);
        tok = scan(is, lstate);
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
    } while( tok->type != END_TOKEN );
    pos = the_node->children;
    the_node->children = pos->siblings;
    free(pos);
    return the_node;
}
