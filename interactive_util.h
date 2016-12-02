/*
 * Copyright (c) 2016 Robert Tate <rob@rtate.se>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef _INERACTIVE_UTIL_H_
#define _INERACTIVE_UTIL_H_

enum command_mode {
	ADD,
	DELETE,
	EXECUTE,
	ECHO,
	EDIT,
	HELP,
	LIST,
	REPLACE,
	SEARCH,
	VERSION,
	COMPLETE,
	NOT_SET
};

typedef struct {
	enum command_mode mode;
	char *name;
	char *file;
	char *description;
	int page;
	size_t arg_size;
	char **arguments;
	int remove_file;
} arg_options_t;

int	parse_args(arg_options_t *, int, char **[]);
int	validate_args(arg_options_t *);


#endif /* _INERACTIVE_UTIL_H_ */
