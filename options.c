#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include "options.h"
#include "gen_plain.h"

void print_usage()
{
    /* TODO: add option output file (stdout by default?) */
    /* TODO: add option verbosity */
    printf(" Usage: inspec [options]\n\n"
            "-t, --target <arg>  Compile to the given tool/file-format\n"
            "                    [plaintext, latex]\n"
            "-v, --verbose[=n]   Output some trace info\n"
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
    char* number_end; /* for strtol() */
    const int MAX_VERBOSE = 42; /* FIXME: what is a reasonable max? */

    while( 1 ) {
        static struct option long_options[] = {
            { "help", no_argument, 0, 'h' },
            { "target", required_argument, 0, 't' },
            { "verbose", optional_argument, 0, 'v' },
            { 0, 0, 0, 0 }
        };
        int option_index = 0;
        int optarg_len = 0;

        c = getopt_long(argc, argv, "ht:v::", long_options, &option_index);
        if( c == -1 ) break;
        switch( c ) {
        case 'h':
            print_usage();
            exit(0);
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
        case 'v':
            if( optarg == NULL )
                opts->verbose = 0;
            else {
                opts->verbose = strtol(optarg, &number_end, 10);
                if( number_end == optarg ) { /* strtol() failed */
                    optarg_len = strlen(optarg);
                    opts->verbose = optarg_len;
                }
            }
            if( opts->verbose > MAX_VERBOSE )
                opts->verbose = MAX_VERBOSE;
            else if( opts->verbose < 0 ) /* possible if strtol() worked */
                opts->verbose = 0;
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
