#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdio.h>

typedef struct input_file {
    FILE* stream;
    char* filename;
    struct input_file* next;
} input_file;

typedef struct conf {
    enum { PLAINTEXT_TARGET, LATEX_TARGET, UNDEFINED_TARGET } target;
    struct input_file* input_files;
} conf;

void print_usage();
int parse_cl_opts(int argc, char* argv[], conf* opts);

#endif // OPTIONS_H_
