#include <stdlib.h>
#include <stdio.h>
#include "options.h"
#include "parse.h"

static int sanity(conf* opts)
{
    if( opts->input_files == NULL ) {
        fprintf(stderr,
                "ERROR:Please specify input files. Use '-' for stdin.\n"
               );
        return 0;
    }
    if( opts->target == UNDEFINED_TARGET ) {
        fprintf(stderr,
                "ERROR:Please specify a valid target to compile to.\n"
                "      Available targets: [plaintext, latex]\n"
               );
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[])
{
    conf opts = { UNDEFINED_TARGET, NULL };
    lex_state lstate = { 1, 0, 0, 1, 0, "?", UNDEFINED_TOKEN, NULL };
    parse_state pstate = { NULL };
    node* rep;
    input_file *file, *tmp_file;


    if( !parse_cl_opts(argc, argv, &opts) || !sanity(&opts) )
        return EXIT_FAILURE;
    file = opts.input_files;
    while( file != NULL ) {
        lstate.filename = file->filename;
        while( ! feof(file->stream) ) {
            rep = parse_text(file->stream, &lstate, &pstate);
            free(rep);
        }
        if( file->stream != stdin ) fclose(file->stream);
        free(file->filename);
        tmp_file = file;
        file = file->next;
        free(tmp_file);
    }
    return EXIT_SUCCESS;
}