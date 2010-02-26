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

#include "../string_ext.h"
#include "cu.h"
#include <ctype.h>

TEST(test_strichr) {
    char* text = "Hello world";
    assertEquals(text+2, strichr(text, 'l'));
    assertEquals(NULL, strichr(text, 'n'));
    assertEquals(text+4, strichr(text, 'O'));
}

TEST(test_strnicmp) {
    assertEquals(0, strnicmp("Hello", "Hello", sizeof("Hello")));
    assertEquals(0, strnicmp("Hello", "HELLO", sizeof("Hello")));
}

TEST(test_stristr) {
    char* text = "Hello world";
    assertEquals(text+7, stristr(text, "or"));
    assertEquals(text+7, stristr(text, "oR"));
    assertEquals(NULL, stristr(text, "oRe"));
}
