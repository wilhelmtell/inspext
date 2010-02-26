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

#include "string_ext.h"
#include <ctype.h>

/* Credits to http://en.wikibooks.org/wiki/C_Programming/Strings */
/* This is a modified version of their strncmp() implementation */
int strnicmp(const char *str1, const char *str2, size_t n)
{
    int c1, c2;

    if (n == 0)
        return 0;
    while (n-- > 0 && tolower(*str1) == tolower(*str2)) {
        if (n == 0 || *str1 == '\0')
            return 0;
        str1++;
        str2++;
    }
    c1 = tolower(*str1);
    c2 = tolower(*str2);
    return (c1 < c2) ? -1 : (c1 > c2);
}

/* Credits to http://en.wikibooks.org/wiki/C_Programming/Strings */
/* This is a modified version of their strchr() implementation */
char* strichr(const char* str, int c)
 {
     c = tolower(c);
     while( *str != '\0' && tolower(*str) != c )
         str++;
     return (tolower(*str) == c) ? (char *)str : NULL;
 }

/* Credits to http://en.wikibooks.org/wiki/C_Programming/Strings */
/* This is a modified version of their strstr() implementation */
char* stristr(const char* str1, const char* str2)
{
    size_t str2len;

    if( *str2 == '\0' )
        return (char *)str1;
    str2len = strlen(str2);
    while( (str1 = strichr(str1, *str2)) != NULL ) {
        if( strnicmp(str1, str2, str2len) == 0 )
            return (char *)str1;
        str1++;
    }
    return NULL;
}
