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
    fprintf(stderr, "Put back %s", token_s(tok->type));
    if( tok->type == CHARACTER_TOKEN && tok->ch == '\n' )
        fprintf(stderr, " NL\n");
    else if( tok->type == CHARACTER_TOKEN )
        fprintf(stderr, " '%c'\n", tok->ch);
    else
        fprintf(stderr, "\n");
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
    int next_ch = peekc(is, state);

    return ch == '\n' && (next_ch == '\n' || prev == UNDEFINED_TOKEN);
}

/* static int is_inside_paragraph(int ch, lex_state* state) */
/* { */
    /* enum token_type prev = state->previous_token; */
    /* return prev == PARAGRAPH_TOKEN || prev == CHARACTER_TOKEN; */
/* } */

static int is_indenting(int ch, lex_state* state)
{
    enum token_type prev = state->previous_token;
    return ch == ' ' && (prev == HEADING_TOKEN || prev == INDENT_TOKEN);
}

/* static int is_at_beginning_of_line(lex_state* state) */
/* { */
    /* int prev_ch = state->previous_token_ch; */
    /* enum token_type prev_type = state->previous_token_type; */
    /* return prev_type == UNDEFINED_TOKEN || (prev_type == CHARACTER_TOKEN && prev_ch == '\n'); */
/* } */

/* TODO: state should hold a copy of the previous token, not just its type */
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
            if( i > state->heading_level )
                ++state->heading_level;
            else
                state->heading_level = i;
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
    } else { /* inside (or about to start) a running text */
        /* A paragraph is a sequence of characters of length 1 character or
         * more, and that spans from the beginning of a line until 2
         * consecutive newlines or until EOF. */
        state->beginning_of_line = 0;
        if( state->delimited ) {
            putbackc(ch, state);
            state->delimited = 0;
            state->previous_token = PARAGRAPH_TOKEN;
            return a_token_of(PARAGRAPH_TOKEN, '\0', state->heading_level);
        } else {
            state->delimited = 0;
            state->previous_token = CHARACTER_TOKEN;
            return a_token_of(CHARACTER_TOKEN, ch, state->heading_level);
        }
    }
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
    else if( tok->type == CHARACTER_TOKEN )
        fprintf(stderr, " '%c'\n", tok->ch);
    else
        fprintf(stderr, "\n");
    return tok;
}
