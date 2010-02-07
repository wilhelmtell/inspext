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

/* autogenerated code; edit at your own peril */

#ifndef TEST_STRING_H_
#define TEST_STRING_H_

#include "cu.h"

TEST(test_strichr);
TEST(test_strnicmp);
TEST(test_stristr);

TEST_SUITE(string_suite) {
    TEST_ADD(test_strichr),
    TEST_ADD(test_strnicmp),
    TEST_ADD(test_stristr),
    TEST_SUITE_CLOSURE
};

#endif /* TEST_STRING_H_ */
