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

#include "cu.h"
#include "../scan.h"
#include <string.h>
#include <stdlib.h>

TEST(token_s_correct_return) {
    assertEqualsM(0, strncmp("HEADING_TOKEN",
                             token_s(HEADING_TOKEN),
                             sizeof("HEADING_TOKEN")),
                  "\"HEADING_TOKEN\" != token_s(HEADING_TOKEN)");
    assertEqualsM(0, strncmp("CHARACTER_TOKEN",
                             token_s(CHARACTER_TOKEN),
                             sizeof("CHARACTER_TOKEN")),
                  "\"CHARACTER_TOKEN\" != token_s(CHARACTER_TOKEN)");
    assertEqualsM(0, strncmp("PARAGRAPH_TOKEN",
                             token_s(PARAGRAPH_TOKEN),
                             sizeof("PARAGRAPH_TOKEN")),
                  "\"PARAGRAPH_TOKEN\" != token_s(PARAGRAPH_TOKEN)");
    assertEqualsM(0, strncmp("INDENT_TOKEN",
                             token_s(INDENT_TOKEN),
                             sizeof("INDENT_TOKEN")),
                  "\"INDENT_TOKEN\" != token_s(INDENT_TOKEN)");
    assertEqualsM(0, strncmp("END_TOKEN",
                             token_s(END_TOKEN),
                             sizeof("END_TOKEN")),
                  "\"END_TOKEN\" != token_s(END_TOKEN)");
    assertEqualsM(0, strncmp("UNDEFINED_TOKEN",
                             token_s(UNDEFINED_TOKEN),
                             sizeof("UNDEFINED_TOKEN")),
                  "\"UNDEFINED_TOKEN\" != token_s(UNDEFINED_TOKEN)");
    assertEqualsM(0, strncmp("illegal token",
                             token_s(-1),
                             sizeof("illegal token")),
                  "\"illegal token\" != token_s(-1)");
}

TEST(putback_puts_back) {
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
    token tok = {
        /* token_type    */ PARAGRAPH_TOKEN,
        /* ch            */ '\0',
        /* heading_level */ 0
    };
    putback(&tok, &lstate);
    assertNotEquals(NULL, lstate.token_buf);
    assertEqualsM(&tok, lstate.token_buf->tok,
            "token tok isn't (the one) in lstate's token_buf");
}

TEST(putback_doesnt_corrupt_token_buf) {
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
    token tok1 = {
        /* token_type    */ PARAGRAPH_TOKEN,
        /* ch            */ '\0',
        /* heading_level */ 0
    };
    token tok2 = {
        /* token_type    */ CHARACTER_TOKEN,
        /* ch            */ 'T',
        /* heading_level */ 0
    };
    putback(&tok1, &lstate);
    putback(&tok2, &lstate);
    assertNotEquals(NULL, lstate.token_buf);
    assertEqualsM(&tok2, lstate.token_buf->tok,
            "token tok2 isn't at the top of lstate's token_buf");
    assertNotEquals(NULL, lstate.token_buf->next);
    assertEqualsM(&tok1, lstate.token_buf->next->tok,
            "token tok1 isn't the 2nd node in lstate's token_buf");
}
