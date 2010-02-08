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

# Specify the project's name
PROJECT_NAME = "INSPext"
# Specify the main target
TARGET = inspc
# Default build type
TYPE = debug
# Which directories contain source files
DIRS = .
# Which libraries are linked
LIBS =
# Dynamic libraries
DLIBS =

# Add directories to the include and library paths
INCPATH = $(HOME)/Development/include
LIBPATH =

# Which files to add to backups, apart from the source code
EXTRA_FILES = Makefile README grammar
# The compiler
CC = gcc

# filename-friendly project name
PROJECT_FILENAME = $(shell echo $(PROJECT_NAME) | tr -d -C 'a-zA-Z0-9 _-' |tr 'A-Z ' 'a-z_')
# Where to store object and dependancy files.
STORE = .make-$(TYPE)
# Makes a list of the source (.c) files.
SOURCE := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.c))
# List of header files.
HEADERS := $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.h))
# Makes a list of the object files that will have to be created.
OBJECTS := $(addprefix $(STORE)/, $(SOURCE:.c=.o))
# Same for the .d (dependancy) files.
DFILES := $(addprefix $(STORE)/,$(SOURCE:.c=.d))

# function for reversing a list. this should be a standard function ...
reverse = $(if $(1),$(call reverse,$(wordlist 2,$(words $(1)),$(1)))) $(firstword $(1))

# The next blocks change some variables depending on the build type
ifeq ($(TYPE),debug)
LDPARAM =
CCPARAM = -Wall -pedantic -ansi -g3
MACROS =
endif

ifeq ($(TYPE),profile)
LDPARAM = -pg /lib/libc.so.5
CCPARAM = -Wall -pedantic -ansi -pg
MACROS = NDEBUG
endif

ifeq ($(TYPE),release)
LDPARAM = -s
CCPARAM = -O2
MACROS = NDEBUG
endif

ifeq ($(TYPE),check)
LDPARAM =
CCPARAM = -Wall -pedantic -ansi -g3
MACROS =
DIRS = . test
TARGET = check
# Unit-test source includes test/main.c instead of ./main.c. We also have to
# specify test/main.c explicitly, not with a wildcard, because it initially
# doesn't exist; it will be auto-generated during the build.
SOURCE := $(filter-out ./main.c, $(foreach DIR,$(DIRS),$(wildcard $(DIR)/*.c)) test/main.c)
OBJECTS := $(addprefix $(STORE)/, $(SOURCE:.c=.o))
TEST_SOURCE := $(wildcard test/test_*.c)
TEST_HEADERS := $(TEST_SOURCE:.c=.h)
endif

# Specify phony rules. These are rules that are not real files.
.PHONY: all clean_check clean distclean backup

# Main target. The @ in front of a command prevents make from displaying
# it to the standard output.
all: $(TARGET)
$(TARGET): $(OBJECTS)
	@echo " LD	$(TARGET)"
	@$(CC) -o $(TARGET) $(OBJECTS) $(LDPARAM) $(foreach LIBRARY, \
		$(LIBS),-l$(LIBRARY)) $(foreach LIB,$(LIBPATH),-L$(LIB))

test/test_%.h: test/test_%.c
	@-echo " GEN	$@"
	@-cd test && ./gen_test_h.sh $(notdir $<) >$(notdir $@)

test/main.c: $(TEST_HEADERS)
	@-echo " GEN	$@"
	@-cd test && ./gen_test_main.sh $(foreach FILE,$^,$(notdir $(FILE))) >$(notdir $@) && cd ..

# Rule for creating object file and .d file, the sed magic is to add
# the object path at the start of the file because the files gcc
# outputs assume it will be in the same dir as the source file.
$(STORE)/%.o: %.c
	@-[ -d $(dir $@) ] || mkdir -p $(dir $@);
	@echo " CC	$?"
	@$(CC) -Wp,-MMD,$(STORE)/$*.dd $(CCPARAM) $(foreach INC,$(INCPATH),-I$(INC)) \
		$(foreach MACRO,$(MACROS),-D$(MACRO)) -c $< -o $@
	@sed -e '1s/^\(.*\)$$/$(subst /,\/,$(dir $@))\1/' $(STORE)/$*.dd > $(STORE)/$*.d
	@rm -f $(STORE)/$*.dd

# Empty rule to prevent problems when a header is deleted.
%.h: ;

clean_check:
	@-echo " RM	test/tmp.*.out"
	@-rm -f test/tmp.*.out
	@-echo " RM	test/tmp.*.err"
	@-rm -f test/tmp.*.err
	@-echo " RM	test/test_*.h"
	@-rm -f test/test_*.h
	@-echo " RM	test/main.c"
	@-rm -f test/main.c

# Cleans up the objects, .d files and executables.
# FIXME: doesn't consider make TYPE=check (we can have multiple stores)
clean: clean_check
	@-$(foreach DIR,$(DIRS),echo " RM	$(STORE)/$(DIR)/*.d"; \
		rm -f $(STORE)/$(DIR)/*.d; \
		echo " RM	$(STORE)/$(DIR)/*.o"; \
		rm -f $(STORE)/$(DIR)/*.o)
	@-$(foreach DIR,$(call reverse, $(sort $(patsubst .,"",$(DIRS)))),if [ -d $(STORE)/$(DIR) ]; \
		then echo " RM	$(STORE)/$(DIR)"; rmdir $(STORE)/$(DIR); fi; )

# FIXME: doesn't consider make TYPE=check (we can have multiple targets)
distclean: clean
	@echo " RM	$(TARGET)"
	@-rm -f $(TARGET)

# Backup the source files.
# FIXME: when TYPE=check backup is broken: SOURCE doesn't include test code
backup:
	@-if [ ! -e .backup ]; then mkdir .backup; fi;
	@if [ -e $(PROJECT_FILENAME) ]; then echo "Directory $(PROJECT_FILENAME) already exists." >&2; false; fi;
	@mkdir $(PROJECT_FILENAME)
	@cp --archive --parents $(SOURCE) $(HEADERS) $(EXTRA_FILES) $(PROJECT_FILENAME)
	@tar c $(PROJECT_FILENAME) |gzip -9 >.backup/backup_`date +%Y%m%d%H%M`.tar.gz
	@rm -rf $(PROJECT_FILENAME)

# Includes the .d files so it knows the exact dependencies for every
# source.
-include $(DFILES)
