#include "gen_plain.h"
#include <assert.h>
#include <ctype.h>

static void gen_plain_heading(FILE* os, node* syntree)
{
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
    while( child != NULL ) {
        if( child->type == HEADING_NODE ) {
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
