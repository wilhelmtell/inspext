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

#ifndef SCAN_H_
#define SCAN_H_

#include <stdio.h>

typedef struct stream_buf_t {
    int ch;
    struct stream_buf_t* next;
} stream_buf_t;

typedef struct token {
    enum token_type {
        HEADING_TOKEN,
        CHARACTER_TOKEN,
        PARAGRAPH_TOKEN,
        INDENT_TOKEN,
        END_TOKEN,
        UNDEFINED_TOKEN
    } type;
    char ch;
    int heading_level;
} token;

typedef struct token_buf_t {
    token* tok;
    struct token_buf_t* next;
} token_buf_t;

typedef struct lex_state {
    int beginning_of_line;
    int delimited; /* two newlines consumed; ready for paragraph or heading */
    int lineno;
    int heading_level;
    char* filename;
    token* previous_token;
    stream_buf_t* stream_buf;
    token_buf_t* token_buf;
} lex_state;

char* token_s(enum token_type t);
token* peek(FILE* is, lex_state* state);
void putback(token* tok, lex_state* lstate);
token* scan(FILE* is, lex_state* lstate);
token* a_token(void);

#endif /* SCAN_H_ */
