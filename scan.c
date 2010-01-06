#include "scan.h"
#include <stdio.h>
#include <stdlib.h>

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
    } else if( ch == '-' || ch == '*' ) { /* intentional fall-through */
        if( state->beginning_of_line );
    } else if( ch >= '0' && ch <= '9' ) { /* intentional fall-throughs */
        if( state->beginning_of_line );
    } else {
        state->beginning_of_line = 0;
        tok->type = CHARACTER;
        tok->ch = ch;
    }
    tok->level = state->level;
    return tok;
}
