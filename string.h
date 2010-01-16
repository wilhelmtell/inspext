#ifndef STRING_H_
#define STRING_H_

#include <string.h>

/* extension for the standard string.h header */

int strnicmp(const char *s1, const char *s2, size_t n);
char* strichr(const char* str, int c);
char* stristr(const char* str1, const char* str2);

#endif // STRING_H_
