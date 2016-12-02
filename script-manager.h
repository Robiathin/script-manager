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

#ifndef _SCRIPT_MANAGER_H_
#define _SCRIPT_MANAGER_H_

#define SM_VERSION "1.1.0"

#define SCRIPT_DB_DIR "/.script-db"
#define SCRIPT_DB_FILE "/script-manager.db"
#define SCRIPT_TABLE "scripts"

#define SM_DEFAULT_EDITOR  "vi"
#define SM_DEFAULT_PAGER   "less"
#define SM_EDITOR_ENV      "SMEDITOR"
#define SM_PAGER_ENV       "SMPAGER"

#define GET_INT_SIZE(x) ((int) floor(log10(abs(x))) + 1)

static void	exit_cleanup(void);
static void	print_usage(void);
static void	print_version(void);
static int	init_sm(void);
static int	add_script(void);
static int	delete_script(void);
static int	execute_script(void);
static int	edit_script(void);
static int	replace_script(void);
static int	search_script(void);
static int	list_script(void);
static int	echo_script(void);
static int	list_script_callback(void *, int, char **, char **);
static int	auto_complete_list(void);
static int	auto_complete_list_callback(void *, int, char **, char **);

#endif /* _SCRIPT_MANAGER_H_ */
