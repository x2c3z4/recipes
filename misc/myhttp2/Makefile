# Alpaca Makefile
# Copyright (C) 2013 vonnyfly <lifeng1519 at gmail dot com>
# This file is released under the BSD license, see the COPYING file
#
# The Makefile composes the final FINAL_CFLAGS and FINAL_LDFLAGS using
# what is needed for ALPACA plus the standard CFLAGS and LDFLAGS passed.
# However when building the dependencies (Jemalloc ...)
# CFLAGS and LDFLAGS are propagated to the dependencies, so to pass
# flags only to be used when compiling / linking ALPACA itself ALPACA_CFLAGS
# and ALPACA_LDFLAGS are used instead (this is the case of 'make gcov').
#
# Dependencies are stored in the Makefile.dep file. To rebuild this file
# Just use 'make dep', but this is only needed by developers.

#release_hdr := $(shell sh -c './mkreleasehdr.sh')
uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
OPTIMIZATION?=-O2

# Default settings
#CC=clang
CC=gcc
STD=-std=c99 -pedantic
WARN=-Wall
OPT=$(OPTIMIZATION)

PREFIX?=/usr/local
INSTALL_BIN=$(PREFIX)/bin
INSTALL=install

# Override default settings if possible
-include .make-settings

#ALPACA_CFLAGS=-DDEBUG
FINAL_CFLAGS=$(STD) $(WARN) $(OPT) $(DEBUG) $(CFLAGS) $(ALPACA_CFLAGS)
FINAL_LDFLAGS=$(LDFLAGS) $(ALPACA_LDFLAGS) $(DEBUG)
FINAL_LIBS=-lm
DEBUG=-g -ggdb -finstrument-functions

FINAL_LDFLAGS+= -rdynamic
FINAL_LIBS+= -pthread


ALPACA_CC=$(QUIET_CC)$(CC) $(FINAL_CFLAGS)
ALPACA_LD=$(QUIET_LINK)$(CC) $(FINAL_LDFLAGS)
ALPACA_INSTALL=$(QUIET_INSTALL)$(INSTALL)

CCCOLOR="\033[34m"
LINKCOLOR="\033[34;1m"
SRCCOLOR="\033[33m"
BINCOLOR="\033[37;1m"
MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

ifndef V
QUIET_CC = @printf '    %b %b\n' $(CCCOLOR)CC$(ENDCOLOR) $(SRCCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_LINK = @printf '    %b %b\n' $(LINKCOLOR)LINK$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
QUIET_INSTALL = @printf '    %b %b\n' $(LINKCOLOR)INSTALL$(ENDCOLOR) $(BINCOLOR)$@$(ENDCOLOR) 1>&2;
endif

ALPACA_SERVER_NAME=alpaca-server
ALPACA_SERVER_OBJ=httpd.o main.o debug.o utils.o
all: $(ALPACA_SERVER_NAME)
	@echo ""
	@echo "Hint: run ./$(ALPACA_SERVER_NAME) ;)"
	@echo ""

.PHONY: all


dep:
	$(ALPACA_CC) -MM *.c > Makefile.dep

.PHONY: dep

# Deps (use make dep to generate this)
include Makefile.dep

persist-settings: distclean
	echo STD=$(STD) >> .make-settings
	echo WARN=$(WARN) >> .make-settings
	echo OPT=$(OPT) >> .make-settings
	echo MALLOC=$(MALLOC) >> .make-settings
	echo CFLAGS=$(CFLAGS) >> .make-settings
	echo LDFLAGS=$(LDFLAGS) >> .make-settings
	echo ALPACA_CFLAGS=$(ALPACA_CFLAGS) >> .make-settings
	echo ALPACA_LDFLAGS=$(ALPACA_LDFLAGS) >> .make-settings
	echo PREV_FINAL_CFLAGS=$(FINAL_CFLAGS) >> .make-settings
	echo PREV_FINAL_LDFLAGS=$(FINAL_LDFLAGS) >> .make-settings

.PHONY: persist-settings

# Prerequisites target
.make-prerequisites:
	@touch $@

# Clean everything, persist settings and build dependencies if anything changed
ifneq ($(strip $(PREV_FINAL_CFLAGS)), $(strip $(FINAL_CFLAGS)))
.make-prerequisites: persist-settings
endif

ifneq ($(strip $(PREV_FINAL_LDFLAGS)), $(strip $(FINAL_LDFLAGS)))
.make-prerequisites: persist-settings
endif

# alpaca-server
$(ALPACA_SERVER_NAME): $(ALPACA_SERVER_OBJ)
	$(ALPACA_LD) -o $@ $^ $(FINAL_LIBS)

# Because the jemalloc.h header is generated as a part of the jemalloc build,
# building it should complete before building any other object. Instead of
# depending on a single artifact, build all dependencies first.
%.o: %.c .make-prerequisites
	$(ALPACA_CC) -c $<

clean:
	rm -rf $(ALPACA_SERVER_NAME) *.o *.gcda *.gcno *.gcov ALPACA.info lcov-html

.PHONY: clean

distclean: clean
	-(rm -f .make-*)

.PHONY: distclean

test: $(ALPACA_SERVER_NAME) $(ALPACA_CHECK_AOF_NAME)
	@(cd ..; ./runtest)

check: test

lcov:
	$(MAKE) gcov
	@(set -e; cd ..; ./runtest --clients 1)
	@geninfo -o ALPACA.info .
	@genhtml --legend -o lcov-html ALPACA.info

.PHONY: lcov

32bit:
	@echo ""
	@echo "WARNING: if it fails under Linux you probably need to install libc6-dev-i386"
	@echo ""
	$(MAKE) CFLAGS="-m32" LDFLAGS="-m32"

gcov:
	$(MAKE) ALPACA_CFLAGS="-fprofile-arcs -ftest-coverage -DCOVERAGE_TEST" ALPACA_LDFLAGS="-fprofile-arcs -ftest-coverage"

noopt:
	$(MAKE) OPTIMIZATION="-O0"

valgrind:
	$(MAKE) OPTIMIZATION="-O0" MALLOC="libc"

src/help.h:
	@../utils/generate-command-help.rb > help.h

install: all
	@mkdir -p $(INSTALL_BIN)
	$(ALPACA_INSTALL) $(ALPACA_SERVER_NAME) $(INSTALL_BIN)
	$(ALPACA_INSTALL) $(ALPACA_BENCHMARK_NAME) $(INSTALL_BIN)
	$(ALPACA_INSTALL) $(ALPACA_CLI_NAME) $(INSTALL_BIN)
	$(ALPACA_INSTALL) $(ALPACA_CHECK_DUMP_NAME) $(INSTALL_BIN)
	$(ALPACA_INSTALL) $(ALPACA_CHECK_AOF_NAME) $(INSTALL_BIN)
