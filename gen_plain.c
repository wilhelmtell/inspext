#include "gen_plain.h"
#include <assert.h>
#include <ctype.h>

static void gen_plain_heading(FILE* os, node* syntree)
{
    /* TODO: separate heading from previous paragraph by 3 newlines. */
    /* How many newlines should we put for seperating one heading from
     * another? */
    while( syntree != NULL ) {
        assert(syntree->type == CHARACTER_NODE);
        fputc(toupper(syntree->ch), os);
        syntree = syntree->siblings;
    }
}

static void gen_plain_paragraph(FILE* os, node* syntree)
{
    while( syntree != NULL ) {
        assert(syntree->type == CHARACTER_NODE);
        fputc(syntree->ch, os);
        syntree = syntree->siblings;
    }
}

void gen_plain(FILE* os, node* syntree)
{
    node *child;
    int i = 0;

    assert(syntree->type == TEXT_NODE);
    child = syntree->children;
    /* generate first heading or paragraph; we don't output a preceding newline
     * so text starts right away, at the beginning of the file. */
    if( child->type == HEADING_NODE ) {
        if( child->type == HEADING_NODE ) {
            for( i = 0; i < child->heading_level; ++i ) {
                fputc(' ', os);
            }
            gen_plain_heading(os, child->children);
        }
    } else if( child->type == PARAGRAPH_NODE ) {
        gen_plain_paragraph(os, child->children);
    }
    child = child->siblings;
    /* subsequent headings/paragraphs: each is preceded by a couple of
     * delimiting newlines. */
    while( child != NULL ) {
        if( child->type == HEADING_NODE ) {
            fputc('\n', os);
            fputc('\n', os);
            for( i = 0; i < child->heading_level; ++i ) {
                fputc(' ', os);
            }
            gen_plain_heading(os, child->children);
        } else if( child->type == PARAGRAPH_NODE) {
            fputc('\n', os);
            fputc('\n', os);
            gen_plain_paragraph(os, child->children);
        }
        child = child->siblings;
    }
}
