#include "string.h"
#include "../string.h"
#include "cu.h"
#include <string.h>
#include <ctype.h>

TEST(strichr_lowercase) {
    char* text = "Hello world";
    assertEquals(text+2, strichr(text, 'l'));
}

TEST(strichr_fail) {
    char* text = "Hello world";
    assertEquals(NULL, strichr(text, 'n'));
}

TEST(strichr_uppercase) {
    char* text = "Hello world";
    assertEquals(text+4, strichr(text, 'O'));
}

int strnicmp(const char *s1, const char *s2, size_t n);

TEST(strnicmp_same_case) {
    assertEquals(0, strnicmp("Hello", "Hello", sizeof("Hello")));
}

TEST(strnicmp_different_case) {
    assertEquals(0, strnicmp("Hello", "HELLO", sizeof("Hello")));
}
