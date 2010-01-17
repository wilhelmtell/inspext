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

#include "gen_latex.h"
#include <assert.h>

static void gen_latex_section(FILE* os, node* syntree, int depth)
{
    assert(depth > 0 && depth < 4);
    fprintf(os, "\n\n");
    switch( depth ) {
    case 1:
        fprintf(os, "\\section{");
        break;
    case 2:
        fprintf(os, "\\subsection{");
        break;
    case 3:
        fprintf(os, "\\subsubsection{");
        break;
    }
    while( syntree != NULL ) {
        assert(syntree->type == CHARACTER_NODE);
        fputc(syntree->ch, os);
        syntree = syntree->siblings;
    }
    fprintf(os, "}");
}

static void gen_latex_paragraph(FILE* os, node* syntree)
{
    while( syntree != NULL ) {
        assert(syntree->type == CHARACTER_NODE);
        fputc(syntree->ch, os);
        syntree = syntree->siblings;
    }
}

void gen_latex(FILE* os, node* syntree)
{
    node *child;

    assert(syntree->type == TEXT_NODE);
    fprintf(os, "\\documentclass[letterpaper,11pt]{article}\n");
    fputc('\n', os);
    fprintf(os, "\\begin{document}");
    child = syntree->children;
    if( child->children == NULL )
        fputc('\n', os);
    /* subsequent headings/paragraphs: each is preceded by a couple of
     * delimiting newlines. */
    while( child != NULL ) {
        if( child->type == HEADING_NODE ) {
            gen_latex_section(os, child->children, child->heading_level);
        } else if( child->type == PARAGRAPH_NODE) {
            fputc('\n', os);
            fputc('\n', os);
            gen_latex_paragraph(os, child->children);
        }
        child = child->siblings;
    }
    fprintf(os, "\n\\end{document}\n");
}
