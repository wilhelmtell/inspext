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

/* sip (get) a character from the state's buffer, or if the buffer is empty --
 * from the given input stream. */
static int sipc(FILE* is, lex_state* state)
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

static token* force_scan(FILE* is, lex_state* state)
{
    int ch = 0, i = 0;

    ch = sipc(is, state);
    if( ch == EOF )
        return a_token_of(END_TOKEN, '\0', state->heading_level);

    /* A heading is an indented text of length 1 character or more, and that
     * spans until 2 consecutive newlines or until EOF. */
    if( ch == ' ' && state->beginning_of_line ) {
        for( i = 1; ch == ' '; ++i )
            ch = sipc(is, state);
        if( ch == EOF || ch == '\n' ) { /* empty line */
            putbackc(ch, state);
            return force_scan(is, state); /* whitespace ignored */
        } else {
            putbackc(ch, state);
            if( i > state->heading_level )
                ++state->heading_level;
            else
                state->heading_level = i;
            while( i-- > 0 )
                putbackc(' ', state);
            return a_token_of(HEADING_TOKEN, '\0', state->heading_level);
        }
    }

    /* A paragraph is a sequence of characters of length 1 character or more,
     * and that spans until 2 consecutive newlines or until EOF. */

    return NULL;
}

/* FIXME: should be static.  static omitted to relax compiler warning */
token* force_scan_old(FILE* is, lex_state* state)
{
    int ch;
    int i = 0, j = 0;
    token* tok;

    ch = sipc(is, state);
    tok = (token*)malloc(sizeof(token));
    if( ch == EOF ) {
        tok->type = END_TOKEN;
    } else if( ch == ' ' ) {
        if( state->beginning_of_line ) {
            /* verify it's a heading */
            for( i = 0; ch == ' '; ++i ) { /* is there text after indent? */
                ch = sipc(is, state);
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
                    ch = sipc(is, state);
                }
                if( i > state->heading_level )
                    i = ++state->heading_level;
                else /* FIXME: trap inconsistent indents */
                    state->heading_level = i;
                tok->heading_level = i;
                putbackc(ch, state);
                while( i-- > 0 ) { /* restore input: putback indent */
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
        ch = sipc(is, state);
        if( ch == '\n' ) { /* heading ahead? */
            for( i = 0; ch == '\n'; ++i ) {
                ch = sipc(is, state);
                for( j = 0; ch == ' '; ++j )
                    ch = sipc(is, state);
                if( ch == '\n' && j > 0 ) { /* line of whitespace only */
                    putbackc('\n', state);
                } else if( ch != '\n' && j > 0 ) { /* heading ahead */
                    putbackc(ch, state);
                    while( j-- > 0 )
                        putbackc(' ', state);
                    ch = ' ';
                    tok->type = HEADING_TOKEN;
                    state->previous_token = HEADING_TOKEN;
                    state->paragraph_separator = 0;
                } else if( ch == '\n' && i > 0 ) { /* j == 0 */
                    putbackc(ch, state);
                    while( i-- > 0 )
                        putbackc('\n', state);
                    ch = '\n';
                }
            }
        }
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
    tok = force_scan(is, state);
    putbackc(tok->ch, state);
    return tok;
}

token* scan(FILE* is, lex_state* lstate)
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
        tok = force_scan(is, lstate);
    }
    fprintf(stderr, "Got %s", token_s(tok->type));
    if( tok->type == CHARACTER_TOKEN && tok->ch == '\n' )
        fprintf(stderr, " NL\n");
    else fprintf(stderr, " '%c'\n", tok->ch);
    return tok;
}
