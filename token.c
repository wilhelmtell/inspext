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
