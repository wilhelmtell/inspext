#ifndef TOKEN_H_
#define TOKEN_H_

typedef struct token {
    enum token_type {
        HEADING_TOKEN,
        CHARACTER_TOKEN,
        PARAGRAPH_TOKEN,
        INDENT_TOKEN,
        END_TOKEN,
        UNDEFINED_TOKEN
    } type;
    char ch;
    int heading_level;
} token;

char* token_s(enum token_type t);

#endif // TOKEN_H_
