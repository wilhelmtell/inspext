#include "gen_latex.h"
#include <assert.h>

/* TODO: implement */
static void gen_latex_heading1(FILE* os, node* syntree)
{
}

/* TODO: implement */
static void gen_latex_heading2(FILE* os, node* syntree)
{
}

/* TODO: implement */
static void gen_latex_heading3(FILE* os, node* syntree)
{
}

/* TODO: implement */
static void gen_latex_heading4(FILE* os, node* syntree)
{
}

/* TODO: implement */
static void gen_latex_paragraph(FILE* os, node* syntree)
{
}

/* TODO: implement */
void gen_latex(FILE* os, node* syntree)
{
    node *child;

    assert(syntree->type == TEXT_NODE);
    fprintf(os, "\\documentclass{article}[letterpaper,11pt]\n");
    fputc('\n', os);
    fprintf(os, "\\begin{document}\n");
    fputc('\n', os);
    child = syntree->children;
    /* subsequent headings/paragraphs: each is preceded by a couple of
     * delimiting newlines. */
    while( child != NULL ) {
        if( child->type == HEADING_NODE ) {
            if( child->heading_level == 1 )
                /* TODO: section? title? */
                gen_latex_heading1(os, child->children);
            else if( child->heading_level == 2 )
                /* TODO: subsection */
                gen_latex_heading2(os, child->children);
            else if( child->heading_level == 3 )
                /* TODO: subsubsection */
                gen_latex_heading3(os, child->children);
            else
                /* TODO: now what? */
                gen_latex_heading4(os, child->children);
        } else if( child->type == PARAGRAPH_NODE) {
            fputc('\n', os);
            fputc('\n', os);
            gen_latex_paragraph(os, child->children);
        }
        child = child->siblings;
    }
    fprintf(os, "\\end{document}\n");
}
