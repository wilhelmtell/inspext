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

#ifndef PARSE_H_
#define PARSE_H_

#include <stdio.h>
#include "scan.h"

typedef struct node {
    enum {
        TEXT_NODE,
        HEADING_NODE,
        INDENT_NODE,
        PARAGRAPH_NODE,
        CHARACTER_NODE,
        END_NODE
    } type;
    struct node* children;
    struct node* siblings;
    char ch;
    int heading_level;
} node;

node* parse_text(FILE* is, lex_state* state);
void free_node(node* n);

#endif // PARSE_H_
