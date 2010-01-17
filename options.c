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

#include <stdlib.h>
#include "string.h"
#include <getopt.h>
#include <stdio.h>
#include "options.h"
#include "generators.h"

/* TODO: specify defaults in help message. create config.h ? */
void print_usage()
{
    printf(" Usage: inspc [options]\n\n"
            "-t, --target <arg>  Compile to the given tool/file-format\n"
            "                    [plaintext, latex, html]\n"
            "-v, --verbose[=n]   Output some trace info\n"
            "-o, --output <out>  Output to file <out>. Use - to specify\n"
            "                    stdout (default)\n"
            );
}

int parse_cl_opts(int argc, char* argv[], conf* opts)
{
    int c, success_flag = 1;
    input_file* file, *pos;
    const char * const plaintext = "plaintext";
    const char * const latex = "latex";
    const char * const html = "html";
    const char * filename;
    int filename_len = 0;
    char* number_end; /* for strtol() */

    opts->output_file = stdout; /* default */
    while( 1 ) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "target", required_argument, 0, 't' },
            { "verbose", optional_argument, 0, 'v' },
            { "output", required_argument, 0, 'o' },
            { 0, 0, 0, 0 }
        };
        int option_index = 0;
        int optarg_len = 0;

        c = getopt_long(argc, argv, "ht:v::o:", long_options, &option_index);
        if( c == -1 ) break;
        switch( c ) {
        case 'h':
            print_usage();
            exit(0);
            break;
        case 't':
            optarg_len = strlen(optarg);
            if( stristr(plaintext, optarg) == plaintext ) {
                opts->gen = &gen_plain;
            } else if( stristr(latex, optarg) == latex ) {
                opts->gen = &gen_latex;
            } else if( stristr(html, optarg) == html ) {
                opts->gen = &gen_html;
            } else {
                fprintf(stderr, "ERROR:Unrecognized target %s\n", optarg);
                success_flag = 0;
                opts->gen = NULL;
            }
            break;
        case 'v':
            /* verbose level. --vN or -v[v[...]] */
            if( optarg == NULL ) { /* no arg given, but still -v given */
                opts->verbose = VERBOSE_DEFAULT + 1;
            } else {
                opts->verbose = strtol(optarg, &number_end, 10);
                if( number_end == optarg ) { /* strtol failed: count -vvv... */
                    optarg_len = strlen(optarg);
                    opts->verbose = 1 + optarg_len;
                }
            }
            if( opts->verbose > VERBOSE_DEBUG )
                opts->verbose = VERBOSE_DEBUG;
            else if( opts->verbose < VERBOSE_FATAL )
                opts->verbose = VERBOSE_FATAL;
            break;
        case 'o':
            filename = optarg;
            optarg_len = strlen(optarg);
            if( strncmp(filename, "-", optarg_len) != 0 )
                if( (opts->output_file = fopen(filename, "w")) == NULL ) {
                    fprintf(stderr,
                            "ERROR:Can't open file %s for writing.\n",
                            filename);
                    success_flag = 0;
                }
            break;
        case '?':
            success_flag = 0;
            break;
        default:
            abort();
        }
    }
    opts->input_files = (input_file*)malloc(sizeof(input_file));
    opts->input_files->next = NULL;
    opts->input_files->filename = NULL;
    opts->input_files->stream = NULL;
    pos = opts->input_files;
    while( optind < argc ) { /* files to compile */
        filename = argv[optind];
        filename_len = strlen(filename);
        file = (input_file*)malloc(sizeof(input_file));
        file->filename = (char*)malloc(filename_len+1);
        strncpy(file->filename, filename, filename_len);
        if( strncmp(filename, "-", filename_len) == 0 ) {
            file->stream = stdin;
        } else {
            file->stream = fopen(filename, "r");
            if( file->stream == NULL ) {
                fprintf(stderr, "ERROR:Can't open file %s for reading\n", filename);
                success_flag = 0;
            }
        }
        ++optind;
        if( file->stream != NULL ) {
            pos->next = file;
            pos = pos->next;
        }
    }
    pos = opts->input_files;
    opts->input_files = opts->input_files->next;
    free(pos);
    return success_flag;
}
