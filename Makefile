# Copyright (c) 2016 Robert Tate <rob@rtate.se>
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

SM = sm

CFLAGS = -pipe -std=c99 -O3 -Wall

LDFLAGS = -lsqlite3 -lm

SRCS = \
	script-manager.c \
	file_util.c \
	interactive_util.c

# Use clang if found, otherwise use gcc.
CC ?= $(shell which clang)
ifeq ($(CC),)
	CC = gcc
endif

.PHONY: all clean install uninstall

# Paging doesn't work on macOS. This will be fixed in the future.
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -D NO_PAGE
endif

$(SM): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(LDFLAGS) -o $(SM)

all: $(SM)

clean:
	rm -f sm

install:
	install -m 444 sm.1 /usr/share/man/man1/
	install -m 555 $(SM) /usr/local/bin/

uninstall:
	rm -f /usr/share/man/man1/sm.1
	rm -f /usr/local/bin/sm
