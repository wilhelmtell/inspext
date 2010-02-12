#!/bin/bash
###############################################################################
# Copyright (C) 2010 Matan Nassau
#
# This file is part of INSPext.
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>.
###############################################################################

###############################################################################
# Look for new CU tests and register them.
#
# The script assumes all test files are in the current directory, and that the
# gen_test_h.sh and gen_test_main.sh scripts are executable and in the current
# directory as well. The script will write all output files in the current
# directory.
#
# FIXME: allow all the necessary files and scripts be anywhere. See above.
#
# CU is a unit-testing framework for C; see http://cu.danfis.cz/
###############################################################################

while getopts 'q' OPT; do
  case $OPT in
    q) QUIET='1'
    ;;
    ?) echo "Usage: $(basename $0) [-q]" >&2
    ;;
  esac
done
shift $(($OPTIND - 1))

if [ $# -eq 0 ]; then
  FILENAMES=test*.c
else
  FILENAMES=$(find $* -maxdepth 1 -name 'test*.c' |tr '\n' ' ')
fi
FILENAMES=$(echo "$FILENAMES" | sed 's/^\s\+\|\s\+$//g')

if [ -z "$FILENAMES" ]; then
  echo "$(basename $0):No test*.c files found." >&2
  exit -1
fi

for FILE in $FILENAMES; do
  HFILE=$(echo $FILE |sed 's/\.c$/.h/')
  if [ -z "$QUIET" ]; then
    echo " GEN	$HFILE"
  fi
  ./gen_test_h.sh $FILE >$HFILE
done
FILENAMES=$(echo "$FILENAMES" |sed "s/\.c\( \|$\)/.h /g")
if [ -z "$QUIET" ]; then
  echo " GEN	main.c"
fi
./gen_test_main.sh $FILENAMES >main.c
