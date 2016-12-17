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

#ifndef _SQL_UTIL_H_
#define _SQL_UTIL_H_

#define BEGIN_TRANSACTION(a) sqlite3_exec(a, "BEGIN TRANSACTION;", NULL, NULL, NULL)
#define END_TRANSACTION(a)   sqlite3_exec(a, "END TRANSACTION;", NULL, NULL, NULL)
#define ROLLBACK(a)          sqlite3_exec(a, "ROLLBACK;", NULL, NULL, NULL)
#define P_ERR_SQL(a)         fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(a))
#define P_ERR_SQL_BIND(a)    fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(a))

int	auto_complete_list_callback(void *, int, char **, char **);
int	list_script_callback(void *, int, char **, char **);

#endif
