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
    fprintf(os, "\\documentclass{article}[letterpaper,11pt]\n");
    fputc('\n', os);
    fprintf(os, "\\begin{document}");
    child = syntree->children;
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
    fprintf(os, "\\end{document}\n");
}
