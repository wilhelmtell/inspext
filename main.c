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

typedef enum {
    SANITY_ERROR_NO_INPUT_FILE = 1,
    SANITY_ERROR_INVALID_TARGET
} sanity_error;

static sanity_error sanity_err = 0;

static char* sanity_error_s(sanity_error err)
{
    if( err == SANITY_ERROR_NO_INPUT_FILE )
        return "please specify input files, or '-' for stdin";
    else if( err == SANITY_ERROR_INVALID_TARGET )
        return "please specify a valid target to compile to";
    return 0;
}

static int insanity(conf* opts)
{
    /* FIXME: extract error messages and have main() display them?
     *        {
     *            ...
     *            return ERR_NO_INPUT_FILE;
     *        }
     *        ...
     *        err = sanity(...);
     *        if( err ) {
     *            fprintf(stderr, "%s\n", err_s(err));
     *            return EXIT_FAILURE;
     *        }
     */
    if( opts->input_files == NULL )
        return sanity_err = SANITY_ERROR_NO_INPUT_FILE;
    if( opts->gen == NULL )
        return sanity_err = SANITY_ERROR_INVALID_TARGET;
    return 0;
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
    sanity_error sanerr = 0;
    options_error opterr = 0;


    if( (opterr = parse_cl_opts(argc, argv, &opts)) ) {
        fprintf(stderr, "ERROR:%s\n", options_error_s(opterr));
        return EXIT_FAILURE;
    } else if( (sanerr = insanity(&opts)) ) {
        fprintf(stderr, "ERROR:%s\n", sanity_error_s(sanerr));
        return EXIT_FAILURE;
    }
    file = opts.input_files;
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
