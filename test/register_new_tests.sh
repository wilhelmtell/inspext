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

BIN=$0
BINNAME=$(basename $BIN)
print_usage() {
  echo "Usage: $BINNAME [-q] [-o output_dir]" >&2
  echo >&2
  echo " -o output_dir     Specify output directory" >&2
  echo " -s                Consider only the basename of the output" >&2
  echo "                   directory in the #includes in main.c" >&2
  echo " -p output_prefix  Output directory for regression files" >&2
  echo " -q                Be quiet" >&2
}

OUTPUT_DIR=.
OUTPUT_PREFIX=.
while getopts 'hsp:qo:' OPT; do
  case $OPT in
    h) print_usage
    exit 0
    ;;
    p) OUTPUT_PREFIX=$OPTARG
    ;;
    s) STRIP="-s"
    ;;
    q) QUIET=1
    ;;
    o) OUTPUT_DIR=$(echo -n "$OPTARG" |sed 's/\/*$//')
    if [ ! -d "$OUTPUT_DIR" -o ! -w "$OUTPUT_DIR" -o ! -x "$OUTPUT_DIR" ]; then
      echo "$BINNAME:Can't find writable directory $OUTPUT_DIR" >&2
      exit 1
    fi
    ;;
    ?) print_usage
    exit 1
    ;;
  esac
done
shift $(($OPTIND - 1))

# FIXME: is this really necessary? less code, less bugs
if [ $# -eq 0 ]; then
  FILENAMES=test*.c
else
  FILENAMES=$(find $* -maxdepth 1 -name 'test*.c' |tr '\n' ' ')
fi
FILENAMES=$(echo -n "$FILENAMES" | sed 's/^\s\+\|\s\+$//g')

if [ -z "$FILENAMES" ]; then
  echo "$BINNAME:No test*.c files found." >&2
  exit 1
fi

for FILE in $FILENAMES; do
  HFILE=$(echo -n $FILE |sed 's/\.c$/.h/')
  if [ -z "$QUIET" ]; then
    echo " GEN	$HFILE"
  fi
  ./gen_test_h.sh $FILE >$OUTPUT_DIR/$HFILE
done
ESCAPED_OUTPUT_DIR=$(echo -n $OUTPUT_DIR |sed 's/\//\\\//g')
for FILE in $FILENAMES; do
  FILE=$(echo -n "$FILE" |sed "s/\(.*\)\.c/$ESCAPED_OUTPUT_DIR\/\1.h/")
  TMPFILENAMES="$TMPFILENAMES $FILE"
done
FILENAMES=$TMPFILENAMES
# FIXME: is this really necessary? less code, less bugs
if [ -z "$QUIET" ]; then
  echo " GEN	main.c"
fi
./gen_test_main.sh $STRIP -o $OUTPUT_PREFIX $FILENAMES >$OUTPUT_DIR/main.c
