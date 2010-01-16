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

#include "gen_html.h"
#include <stdio.h>
#include <assert.h>

static void gen_html_heading(FILE* os, node* syntree, int depth)
{
    assert(depth > 0 && depth < 7);
    fprintf(os, "\n  <h%d>", depth);
    while( syntree != NULL ) {
        assert(syntree->type == CHARACTER_NODE);
        fputc(syntree->ch, os);
        syntree = syntree->siblings;
    }
    fprintf(os, "</h%d>", depth);
}

static void gen_html_paragraph(FILE* os, node* syntree)
{
    fprintf(os, "\n\n  <p>\n  ");
    while( syntree != NULL ) {
        assert(syntree->type == CHARACTER_NODE);
        if( syntree->ch == '\n' )
            fprintf(os, "\n  ");
        else
            fputc(syntree->ch, os);
        syntree = syntree->siblings;
    }
    fprintf(os, "\n  </p>");
}

void gen_html(FILE* os, node* syntree)
{
    node *child;

    assert(syntree->type == TEXT_NODE);
    fprintf(os, "<html>\n");
    fprintf(os, "  <head>\n"); /* FIXME: don't hardcode indent */
    fprintf(os, "  </head>\n");
    fputc('\n', os);
    fprintf(os, "  <body>");
    child = syntree->children;
    while( child != NULL ) {
        if( child->type == HEADING_NODE ) {
            gen_html_heading(os, child->children, child->heading_level);
        } else if( child->type == PARAGRAPH_NODE) {
            gen_html_paragraph(os, child->children);
        }
        child = child->siblings;
    }
    fprintf(os, "\n  </body>\n");
    fprintf(os, "</html>\n");
}
