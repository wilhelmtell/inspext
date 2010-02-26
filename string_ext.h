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

#ifndef STRING_EXT_H_
#define STRING_EXT_H_

#include <string.h>

/* extension for the standard string.h header */

int strnicmp(const char *s1, const char *s2, size_t n);
char* strichr(const char* str, int c);
char* stristr(const char* str1, const char* str2);

#endif /* STRING_EXT_H_ */
