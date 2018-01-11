# Copyright (c) 2016-2018 Robert Tate <rob@rtate.se>
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
CFLAGS = -pipe -std=c89 -O3 -Wall -Werror -Wextra -D_BSD_SOURCE
CFLAGS += $(shell pkg-config --cflags sqlite3 2>/dev/null)

LDFLAGS = -lm -lc
LDFLAGS += $(shell pkg-config --libs sqlite3 2>/dev/null || echo '-lsqlite3')

# Paging doesn't work on macOS. This will be fixed in the future (probably, assuming I get around to it).
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CFLAGS += -DNO_PAGE
endif

ifdef NO_PAGE
	CFLAGS += -DNO_PAGE
endif

ifndef WITHOUT_AUTOCOMPLETE
	CFLAGS += -DWITH_AUTOCOMPLETE
endif

OBJS = $(shell ls src/*.c | sed -e 's/^src/obj/' -e 's/\.c$$/.o/')

prefix ?= /usr/local
man_prefix ?= /usr/share

CC ?= clang

.PHONY: clean install uninstall

obj/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXECUTABLE): obj $(OBJS)
	$(CC) -o $(EXECUTABLE) $(OBJS) $(LDFLAGS)
	sed "s/.Nm sm/.Nm $(EXECUTABLE)/" man/sm.1 > $(EXECUTABLE).1

obj:
	mkdir -p obj

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE) $(EXECUTABLE).1 $(OBJS)
	rm -rf ./obj

install: $(EXECUTABLE)
	install -m 444 $(EXECUTABLE).1 ${man_prefix}/man/man1/$(EXECUTABLE).1
	install -m 555 $(EXECUTABLE) ${prefix}/bin/

uninstall:
	rm -f ${man_prefix}/man/man1/$(EXECUTABLE).1
	rm -f ${prefix}/bin/$(EXECUTABLE)
