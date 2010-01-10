#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include "options.h"
#include "gen_plain.h"

void print_usage()
{
    printf("Usage: inspec [options]\n\n"
            "-t, --target <arg>  Compile to the given tool/file-format\n"
            "                    [plaintext, latex]\n"
            );
}

int parse_cl_opts(int argc, char* argv[], conf* opts)
{
    int c, success_flag = 1;
    input_file* file, *pos;
    const char * const plaintext = "plaintext";
    /* const char * const latex = "latex"; */
    const char * filename;
    int filename_len = 0;
    while( 1 ) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "target", required_argument, 0, 't' },
            { 0, 0, 0, 0 }
        };
        int option_index = 0;
        int optarg_len = 0;

        c = getopt_long(argc, argv, "ht:", long_options, &option_index);
        if( c == -1 ) break;
        switch( c ) {
        case 'h':
            print_usage();
            break;
        case 't':
            optarg_len = strlen(optarg);
            if( strstr(plaintext, optarg) == plaintext ) {
                opts->gen = &gen_plain;
            /* } else if( strstr(latex, optarg) == latex ) { */
                /* opts->gen = &gen_latex; */
            } else {
                fprintf(stderr, "ERROR:Unrecognized target %s\n", optarg);
                success_flag = 0;
                opts->gen = NULL;
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
                success_flag = 0; /* FIXME: error when can't open file? */
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
