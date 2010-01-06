#include "scan.h"
#include <stdio.h>
#include <stdlib.h>

token* scan(FILE* is, lex_state* state)
{
    int ch;
    token* tok;

    ch = fgetc(is);
    if( ch == EOF ) return NULL;
    tok = (token*)malloc(sizeof(token));
    tok->level = state->level;
    switch( ch ) {
    case ' ':
        if( state->beginning_of_line ) {
            tok->type = INDENT;
            ++state->level;
        }
        break;
    case '\n':
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
        break;
    case '-': /* intentional fall-through */
    case '*':
        if( state->beginning_of_line )
            break;
    case '0': /* intentional fall-throughs */
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        if( state->beginning_of_line )
            break;
    default:
        state->beginning_of_line = 0;
        tok->type = CHARACTER;
        tok->ch = ch;
    }
    tok->level = state->level;
    return tok;
}
