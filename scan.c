#include "scan.h"
#include <stdio.h>
#include <stdlib.h>

void putback(int ch, lex_state* state)
{
    stream_buf_t* tmp = state->stream_buf;

    state->stream_buf = (stream_buf_t*)malloc(sizeof(stream_buf_t));
    state->stream_buf->ch = ch;
    state->stream_buf->next = tmp;
}

int sip(FILE* is, lex_state* state)
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

token* scan(FILE* is, lex_state* state)
{
    int ch;
    token* tok;

    ch = fgetc(is);
    tok = (token*)malloc(sizeof(token));
    tok->level = state->level;
    if( ch == EOF ) {
        tok->type = END;
    } else if( ch == ' ' ) {
        if( state->beginning_of_line ) {
            tok->type = INDENT;
            ++state->level;
        }
    } else if( ch == '\n' ) {
        state->beginning_of_line = 1;
        state->level = 0;
        ++state->lineno;
        ch = fgetc(is);
        if( ch == '\n' ) {
            while( ch != EOF && ch == '\n' ) ch = fgetc(is);
            if( ch == EOF ) tok->type = END;
            else {
                fputc(ch, is);
                tok->type = PARAGRAPH;
            }
        } else {
            fputc(ch, is);
            tok->type = CHARACTER;
            tok->ch = ch;
        }
    } else {
        state->beginning_of_line = 0;
        tok->type = CHARACTER;
        tok->ch = ch;
    }
    tok->level = state->level;
    return tok;
}
