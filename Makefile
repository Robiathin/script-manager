# Copyright (c) 2016-2017 Robert Tate <rob@rtate.se>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

EXECUTABLE ?= sm

CFLAGS = -pipe -std=c99 -O3 -fgnu89-inline -Wall -Werror
LDFLAGS = -lsqlite3 -lm

SRCS = \
	script-manager.c \
	file_util.c \
	interactive_util.c \
	sql_util.c

# If CC isn't set, use clang if found, otherwise use gcc.
CC ?= $(shell which clang)
ifeq ($(CC),)
	CC = gcc
endif

.PHONY: all clean install uninstall

# Paging doesn't work on macOS. This will be fixed in the future (probably, assuming I get around to it).
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -DNO_PAGE
endif

$(EXECUTABLE): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $(EXECUTABLE)

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

install:
	install -m 444 sm.1 ${prefix}/usr/share/man/man1/$(EXECUTABLE).1
	install -m 555 $(EXECUTABLE) ${prefix}/usr/local/bin/

uninstall:
	rm -f ${prefix}/usr/share/man/man1/$(EXECUTABLE).1
	rm -f ${prefix}/usr/local/bin/$(EXECUTABLE)
