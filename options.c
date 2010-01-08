#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include "options.h"

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
    input_file* file = NULL;
    const char * const plaintext = "plaintext";
    const char * const latex = "latex";
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
            if( strstr(plaintext, optarg) == plaintext )
                opts->target = PLAINTEXT_TARGET;
            else if( strstr(latex, optarg) == latex )
                opts->target = LATEX_TARGET;
            break;
        case '?':
            success_flag = 0;
            break;
        default:
            abort();
        }
    }
    while( optind < argc ) { /* files to compile */
        filename = argv[optind];
        filename_len = strlen(filename);
        file = opts->input_files;
        opts->input_files = (input_file*)malloc(sizeof(input_file));
        opts->input_files->filename = (char*)malloc(filename_len+1);
        strncpy(opts->input_files->filename, filename, filename_len);
        if( strncmp(argv[optind], "-", strlen(argv[optind])) == 0 ) {
            opts->input_files->stream = stdin;
        } else {
            opts->input_files->stream = fopen(filename, "r");
        }
        ++optind;
        opts->input_files->next = file;
    }
    return success_flag;
}
