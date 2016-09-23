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

#include <sys/stat.h>

#include <stdio.h>

#include "file_util.h"

int
copy_file(char *from, char *to)
{
	FILE *from_file, *to_file;
	from_file = fopen(from, "rb");
	if (from_file == NULL)
		return 1;
	to_file = fopen(to, "wb");
	if (to_file == NULL) {
		fclose(from_file);
		return 1;
	}
	int ch;
	while (1) {
		ch = fgetc(from_file);
		if (feof(from_file))
			break;
		fputc(ch, to_file);
	}
	fclose(from_file);
	fclose(to_file);
	return 0;
}

int
print_file(char *file_path)
{
	FILE *file;
	file = fopen(file_path, "rb");
	if (file == NULL)
		return 1;
	int ch;
	while (1) {
		ch = fgetc(file);
		if (feof(file))
			break;
		printf("%c", ch);
	}
	fclose(file);
	return 0;
}

int
make_executable(char *path)
{
	struct stat file_stat;
	if (stat(path, &file_stat))
		return 1;
	if (chmod(path, file_stat.st_mode | S_IXOTH | S_IXGRP | S_IXUSR))
		return 1;
	return 0;
}
