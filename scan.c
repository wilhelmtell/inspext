#include "scan.h"
#include <stdio.h>
#include <stdlib.h>

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
    /* FIXME: check for NULL */
    stream_buf_t* tmp = state->stream_buf;

    state->stream_buf = (stream_buf_t*)malloc(sizeof(stream_buf_t));
    state->stream_buf->ch = ch;
    state->stream_buf->next = tmp;
}

static int cipc(FILE* is, lex_state* state)
{
    stream_buf_t* tmp;
    int ch;

    /* FIXME: check for NULL */
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

static token* scan(FILE* is, lex_state* state)
{
    int ch;
    int i;
    token* tok;

    ch = cipc(is, state);
    tok = (token*)malloc(sizeof(token));
    if( ch == EOF ) {
        tok->type = END_TOKEN;
    } else if( ch == ' ' ) {
        if( state->beginning_of_line ) {
            /* verify it's a heading */
            for( i = 0; ch == ' '; ++i ) { /* is there text after indent? */
                ch = cipc(is, state);
            }
            putbackc(ch, state);
            while( --i > 0 ) {
                putbackc(' ', state);
            }
            if( ch == EOF || ch == '\n' ) { /* no heading */
                tok->type = CHARACTER_TOKEN;
                tok->ch = ' ';
                state->previous_token = CHARACTER_TOKEN;
                state->indenting = 0;
                state->beginning_of_line = 0;
            } else {
                ch = ' '; /* was changed by heading validation */
                tok->type = HEADING_TOKEN;
                state->previous_token = HEADING_TOKEN;
                state->indenting = 1;
                state->beginning_of_line = 0;
                for( i = 0; ch == ' '; ++i ) { /* calculate level */
                    ch = cipc(is, state);
                }
                if( i > state->heading_level )
                    i = ++state->heading_level;
                else /* FIXME: trap inconsistent indents */
                    state->heading_level = i;
                tok->heading_level = i;
                putbackc(ch, state);
                while( i-- > 0 ) { /* restore input: putbackc indent */
                    putbackc(' ', state);
                }
            }
        } else if( state->indenting ) { /* not beginning_of_line */
            tok->type = INDENT_TOKEN;
            state->previous_token = INDENT_TOKEN;
        } else { /* not beginning_of_line, not indenting */
            tok->type = CHARACTER_TOKEN;
            tok->ch = ch;
        }
    } else if( ch == '\n' ) {
        state->beginning_of_line = 1;
        state->indenting = 0;
        ch = cipc(is, state);
        if( ch == '\n' && ! state->paragraph_separator ) {
            putbackc('\n', state);
            putbackc('\n', state);
            tok->type = PARAGRAPH_TOKEN;
            state->previous_token = PARAGRAPH_TOKEN;
            state->paragraph_separator = 1;
        } else { /* FIXME: treat lines with only whitespace as empty lines */
            putbackc(ch, state);
            ++state->lineno;
            tok->type = CHARACTER_TOKEN;
            state->previous_token = CHARACTER_TOKEN;
            tok->ch = '\n';
        }
    } else {
        state->paragraph_separator = 0;
        tok->type = CHARACTER_TOKEN;
        state->previous_token = CHARACTER_TOKEN;
        state->beginning_of_line = state->indenting = 0;
        tok->ch = ch;
    }
    return tok;
}

token* peek(FILE* is, lex_state* state)
{
    token* tok;

    /* TODO: maintain token buf as opposed to char buf */
    /*       this so i dont need to scan the same token twice */
    tok = scan(is, state);
    putbackc(tok->ch, state);
    return tok;
}

token* sip(FILE* is, lex_state* lstate)
{
    token_buf_t* tmp;
    token* tok;

    /* FIXME: report error when NULL */
    if( lstate->token_buf != NULL ) {
        tmp = lstate->token_buf;
        tok = lstate->token_buf->tok;
        lstate->token_buf = lstate->token_buf->next;
        free(tmp);
    } else {
        tok = scan(is, lstate);
    }
    return tok;
}
