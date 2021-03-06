/*
 * Copyright (c) 2016-2018 Robert Tate <rob@rtate.se>
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

#include <stdio.h>
#include <string.h>

#include "sql.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

#ifdef WITH_AUTOCOMPLETE
int
auto_complete_list_callback(void *not_used, int argc, char **argv, char **column)
{
	int i;

	for (i = 0; i < argc; i++)
		printf("%s ", argv[i]);

	return 0;
}
#endif /* WITH_AUTOCOMPLETE */

int
list_script_callback(void *not_used, int argc, char **argv, char **column)
{
	int i;

	for (i = 1; i < argc; i++)
		printf("%s: %s\n", column[i], argv[i]);

	puts("");

	return 0;
}

#pragma GCC diagnostic pop
