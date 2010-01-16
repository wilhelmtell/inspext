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
