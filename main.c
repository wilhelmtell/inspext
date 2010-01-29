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
#include <stdio.h>
#include "options.h"
#include "parse.h"

conf opts = {
    /* gen         */ NULL,
    /* input_files */ NULL,
    /* output_file */ NULL,
    /* verbose     */ VERBOSE_ERROR
};

static int sanity(conf* opts)
{
    if( opts->input_files == NULL ) {
        fprintf(stderr,
                "ERROR:Please specify input files. Use '-' for stdin.\n"
               );
        return 0;
    }
    if( opts->gen == NULL ) {
        fprintf(stderr,
                "ERROR:Please specify a valid target to compile to.\n"
               );
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[])
{
    lex_state lstate = {
        /* beginning_of_line */ 1,
        /* delimited         */ 0,
        /* line_number       */ 0,
        /* heading_level     */ 0,
        /* filename          */ NULL,
        /* previous_token    */ UNDEFINED_TOKEN,
        /* stream_buf        */ NULL,
        /* token_buf         */ NULL
    };
    node* rep;
    input_file *file, *tmp_file;


    if( !parse_cl_opts(argc, argv, &opts) || !sanity(&opts) )
        return EXIT_FAILURE;
    file = opts.input_files;
    if( file == NULL )
        return EXIT_FAILURE;
    while( file != NULL ) {
        lstate.filename = file->filename;
        while( ! feof(file->stream) ) {
            rep = parse_text(file->stream, &lstate);
            opts.gen(opts.output_file, rep);
            free_node(rep);
        }
        if( file->stream != stdin ) fclose(file->stream);
        free(file->filename);
        tmp_file = file;
        file = file->next;
        free(tmp_file);
    }
    return EXIT_SUCCESS;
}
