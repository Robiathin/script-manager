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
CFLAGS = -pipe -std=c89 -O3 -Wall -Werror -D_BSD_SOURCE
CFLAGS += $(shell pkg-config --cflags sqlite3 2>/dev/null)

# Paging doesn't work on macOS. This will be fixed in the future (probably, assuming I get around to it).
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -DNO_PAGE
endif

LDFLAGS = -lm -lc
LDFLAGS += $(shell pkg-config --libs sqlite3 2>/dev/null || echo '-lsqlite3')

OBJS = $(shell ls *.c | sed 's/\.c$$/.o/')

# If CC is not set, try clang if found, otherwise use gcc.
CC ?= $(shell which clang)
ifeq ($(CC),)
	CC = gcc
endif

.PHONY: clean install uninstall

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECUTABLE): $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(LDFLAGS)

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(OBJS)

install:
	install -m 444 sm.1 ${prefix}/usr/share/man/man1/$(EXECUTABLE).1
	install -m 555 $(EXECUTABLE) ${prefix}/usr/local/bin/

uninstall:
	rm -f ${prefix}/usr/share/man/man1/$(EXECUTABLE).1
	rm -f ${prefix}/usr/local/bin/$(EXECUTABLE)
