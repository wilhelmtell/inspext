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

#include "scan.h"
#include <stdio.h>
#include <stdlib.h>

char* token_s(enum token_type t)
{
    if( t == HEADING_TOKEN ) return "HEADING_TOKEN";
    else if( t == CHARACTER_TOKEN ) return "CHARACTER_TOKEN";
    else if( t == PARAGRAPH_TOKEN ) return "PARAGRAPH_TOKEN";
    else if( t == INDENT_TOKEN ) return "INDENT_TOKEN";
    else if( t == END_TOKEN ) return "END_TOKEN";
    else if( t == UNDEFINED_TOKEN ) return "UNDEFINED_TOKEN";
    else return "illegal token";
}

void putback(token* tok, lex_state* lstate)
{
    token_buf_t* tmp;

    tmp = lstate->token_buf;
    lstate->token_buf = (token_buf_t*)malloc(sizeof(token_buf_t));
    lstate->token_buf->tok = tok;
    lstate->token_buf->next = tmp;
}

static void putbackc(int ch, lex_state* state)
{
    stream_buf_t* tmp = state->stream_buf;

    state->stream_buf = (stream_buf_t*)malloc(sizeof(stream_buf_t));
    state->stream_buf->ch = ch;
    state->stream_buf->next = tmp;
}

static int sipc(FILE* is, lex_state* state)
{
    stream_buf_t* tmp;
    int ch;

    if( state->stream_buf != NULL ) {
        tmp = state->stream_buf;
        ch = state->stream_buf->ch;
        state->stream_buf = state->stream_buf->next;
        free(tmp);
    } else {
        ch = fgetc(is);
    }
    return ch;
}

token* a_token(void)
{
    token* tok = NULL;
    tok = (token*)malloc(sizeof(token));
    tok->type = UNDEFINED_TOKEN;
    tok->ch = 0;
    tok->heading_level = 0;
    return tok;
}

token* a_token_of(enum token_type type, char ch, int heading_level)
{
    token* tok = NULL;
    tok = (token*)malloc(sizeof(token));
    tok->type = type;
    tok->ch = ch;
    tok->heading_level = heading_level;
    return tok;
}

static int peekc(FILE* is, lex_state* state)
{
    int ch = 0;

    ch = sipc(is, state);
    putbackc(ch, state);
    return ch;
}

static int is_leading_or_delimiting_newline(int ch, FILE* is, lex_state* state)
{
    enum token_type prev = state->previous_token;
    int leading_or_trailing;
    int next_ch = peekc(is, state);

    leading_or_trailing = (prev == UNDEFINED_TOKEN || next_ch == EOF);
    return ch == '\n' && (next_ch == '\n' || leading_or_trailing);
}

static int is_indenting(int ch, lex_state* state)
{
    enum token_type prev = state->previous_token;
    return ch == ' ' && (prev == HEADING_TOKEN || prev == INDENT_TOKEN);
}

/* Scan a token from the given input stream.
 *
 * This will not access the token buffer.  scan() is probably the only function
 * that should call force_scan().
 */
static token* force_scan(FILE* is, lex_state* state)
{
    int ch = 0, i = 0;

    ch = sipc(is, state);
    if( ch == EOF ) {
        state->beginning_of_line = 0;
        state->previous_token = END_TOKEN;
        return a_token_of(END_TOKEN, '\0', state->heading_level);
    } else if( ch == ' ' && state->beginning_of_line ) {
        /* A heading is an indented text of length 1 character or more, and
         * that spans from the beginnnig of a line until 2 consecutive newlines
         * or until EOF. */

        /* TODO: allow for multiline heading */
        for( i = 0; ch == ' '; ++i )
            ch = sipc(is, state);
        putbackc(ch, state);
        /* if ch is EOF or newline the line is semantically empty; ignore */
        if( ch == EOF || ch == '\n' ) {
            if( ch == '\n' ) ++state->lineno;
            return force_scan(is, state);
        } else {
            if( i > state->heading_level ) {
                ++state->heading_level;
                i = state->heading_level;
            } else {
                state->heading_level = i;
            }
            while( i-- > 0 )
                putbackc(' ', state);
            state->previous_token = HEADING_TOKEN;
            state->beginning_of_line = 0;
            return a_token_of(HEADING_TOKEN, '\0', state->heading_level);
        }
    } else if( is_indenting(ch, state) ) {
        state->beginning_of_line = 0;
        state->delimited = 0;
        state->previous_token = INDENT_TOKEN;
        return a_token_of(INDENT_TOKEN, '\0', state->heading_level);
    } else if( is_leading_or_delimiting_newline(ch, is, state) ) {
        state->beginning_of_line = 1;
        state->delimited = 1;
        /* leading/delimiting newlines; chop them out */
        while( ch == '\n' ) {
            ++state->lineno;
            ch = sipc(is, state);
        }
        putbackc(ch, state);
        return force_scan(is, state);
    } else if( ch == '\n' && peekc(is, state) == ' ' ) {
        state->beginning_of_line = 1;
        return force_scan(is, state);
    } else { /* inside (or about to start) a running text */
        /* A paragraph is a sequence of characters of length 1 character or
         * more, and that spans from the beginning of a line until 2
         * consecutive newlines or until EOF. */
        state->beginning_of_line = 0;
        if( state->delimited || state->previous_token == UNDEFINED_TOKEN ) {
            putbackc(ch, state);
            state->delimited = 0;
            state->previous_token = PARAGRAPH_TOKEN;
            return a_token_of(PARAGRAPH_TOKEN, '\0', state->heading_level);
        } else {
            state->previous_token = CHARACTER_TOKEN;
            return a_token_of(CHARACTER_TOKEN, ch, state->heading_level);
        }
    }
}

token* peek(FILE* is, lex_state* state)
{
    token* tok;

    tok = force_scan(is, state);
    putbackc(tok->ch, state);
    return tok;
}

/* Scan a token from the input stream.
 *
 * If the token buffer holds a token then return that token. Otherwise scan a
 * token from the given input stream.
 *
 * The state parameter must point to a valid state object.
 */
token* scan(FILE* is, lex_state* lstate)
{
    token_buf_t* tmp;
    token* tok;

    if( lstate->token_buf != NULL ) {
        tmp = lstate->token_buf;
        tok = lstate->token_buf->tok;
        lstate->token_buf = lstate->token_buf->next;
        free(tmp);
    } else {
        tok = force_scan(is, lstate);
    }
    return tok;
}
