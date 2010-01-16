#include "string.h"
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
