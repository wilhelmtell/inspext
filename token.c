#include "token.h"

char* token_s(enum token_type t)
{
    if( t == HEADING_TOKEN ) return "HEADING_TOKEN";
    else if( t == CHARACTER_TOKEN ) return "CHARACTER_TOKEN";
    else if( t == PARAGRAPH_TOKEN ) return "PARAGRAPH_TOKEN";
    else if( t == INDENT_TOKEN ) return "INDENT_TOKEN";
    else if( t == END_TOKEN ) return "END_TOKEN";
    else if( t == UNDEFINED_TOKEN ) return "UNDEFINED_TOKEN";
    else return "illegal token";
}
