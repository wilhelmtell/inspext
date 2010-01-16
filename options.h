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

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include <stdio.h>
#include "parse.h"

typedef struct input_file {
    FILE* stream;
    char* filename;
    struct input_file* next;
} input_file;

typedef struct conf {
    void (*gen)(FILE*,node*);
    struct input_file* input_files;
    FILE* output_file;
    enum verbose_type {
        VERBOSE_DEFAULT = 1,
        VERBOSE_FATAL = 0,
        VERBOSE_ERROR = 1,
        VERBOSE_WARNING,
        VERBOSE_INFO,
        VERBOSE_DEBUG
    } verbose;
} conf;

void print_usage();
int parse_cl_opts(int argc, char* argv[], conf* opts);

#endif // OPTIONS_H_
