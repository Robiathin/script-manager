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
#include <sys/types.h>

#include <unistd.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <sqlite3.h>
#include <getopt.h>

#include "file_util.h"
#include "script-manager.h"

sqlite3 *db;
char *script_path;
struct arg_options args;

int
main(int argc, char * const *argv)
{
	args.mode = NOT_SET;
	args.name = NULL;
	args.file = NULL;
	args.description = NULL;
	args.page = 1;
	args.arg_size = 0;
	args.arguments = NULL;
	args.remove_file = -1;

	atexit(exit_cleanup);

	int error = 0;
	char option;

	while ((option = getopt(argc, argv,"ad:e:E:hkKlpPr:svA:n:f:D:")) != -1) {
		switch (option) {
		case 'a': /* ADD */
			if (args.mode == NOT_SET)
				args.mode = ADD;
			else
				error = 1;

			break;
		case 'd': /* DELETE */
			if (args.mode == NOT_SET) {
				args.mode = DELETE;
			} else {
				error = 1;
				break;
			}

			if (args.name == NULL)
				args.name = optarg;
			else
				error = 1;

			break;
		case 'e': /* EXECUTE */
			if (args.mode == NOT_SET) {
				args.mode = EXECUTE;
			} else {
				error = 1;
				break;
			}

			if (args.name == NULL)
				args.name = optarg;
			else
				error = 1;

			break;
		case 'E': /* ECHO */
			if (args.mode == NOT_SET) {
				args.mode = ECHO;
			} else {
				error = 1;
				break;
			}

			if (args.name == NULL)
				args.name = optarg;
			else
				error = 1;

			break;
		case 'h': /* HELP */
			if (args.mode == NOT_SET)
				args.mode = HELP;
			else
				error = 1;

			break;
		case 'k': /* KEEP FILE */
			if (args.remove_file == -1)
				args.remove_file = 0;
			else
				error = 1;

			break;
		case 'K': /* DON'T KEEP FILE */
			if (args.remove_file == -1)
				args.remove_file = 1;
			else
				error = 1;

			break;
		case 'l':
			if (args.mode == NOT_SET)
				args.mode = LIST;
			else
				error = 1;

			break;
		case 'p': /* DON'T PAGE */
			args.page = 0;
			break;
		case 'P': /* PAGE */
			args.page = 1;
			break;
		case 'r': /* REPLACE */
			if (args.mode == NOT_SET) {
				args.mode = REPLACE;
			} else {
				error = 1;
				break;
			}

			if (args.name == NULL)
				args.name = optarg;
			else
				error = 1;

			break;
		case 's': /* SEARCH */
			if (args.mode == NOT_SET)
				args.mode = SEARCH;
			else
				error = 1;
			break;
		case 'v': /* VERSION */
			if (args.mode == NOT_SET)
				args.mode = VERSION;
			else
				error = 1;
			break;
		case 'A': /* ARGUMENT */
			if (args.arg_size == 0)
				args.arguments = malloc(sizeof(char *));
			else
				args.arguments = realloc(args.arguments, (args.arg_size + 1) * sizeof(char *));

			if (args.arguments == NULL) {
				error = 2;
			} else {
				args.arguments[args.arg_size] = optarg;
				args.arg_size++;
			}

			break;
		case 'n': /* NAME */
			args.name = optarg;
			break;
		case 'f': /* FILE */
			args.file = optarg;
			break;
		case 'D': /* DESCRIPTION */
			args.description = optarg;
			break;
		default:
			error = 1;
		}

		if (error == 1) {
			fprintf(stderr, "Invalid args!\n");
			print_usage();
			return 1;
		} else if (error == 2) {
			fprintf(stderr, "Error allocating memory!\n");
			return 2;
		}
	}

	if (validate_args()) {
		fprintf(stderr, "Invalid arguments!\n\n");
		print_usage();
		return 1;
	}

	int init_result = init_sm();

	if (init_result)
		return init_result;

	switch (args.mode) {
	case ADD:
		return add_script();
	case DELETE:
		return delete_script();
	case EXECUTE:
		return execute_script();
	case ECHO:
		return echo_script();
	case LIST:
		return list_script();
	case REPLACE:
		return replace_script();
	case SEARCH:
		return search_script();
	case HELP:
		print_usage();
		return 0;
	case VERSION:
		print_version();
		return 0;
	default:
		break;
	}

	return 1;
}

static void
exit_cleanup(void)
{
	sqlite3_close(db);
	free(script_path);
	free(args.arguments);
}

static void
print_usage(void)
{
	puts("See man page for more detailed information. Usage options:");
	puts("\tadd:\t\t-a -n name -D description -f file");
	puts("\tdelete:\t\t-d name");
	puts("\texecute:\t-e name [-A arg] ...");
	puts("\thelp:\t\t-h");
	puts("\tlist:\t\t-l");
	puts("\treplace:\t-r name -f file");
	puts("\tsearch:\t\t-s [-n name -D description]");
	puts("\tversion:\t-v");
}

static void
print_version(void)
{
	puts("Script Manager Vesion: " SM_VERSION);
	puts("Copyright (c) 2016 Robert Tate");
	puts("This software is available under the ISC license.");
}

static int
validate_args(void)
{
	if (args.mode == NOT_SET)
		return 1;

	if (args.mode == ADD && (args.name == NULL || args.file == NULL
	    || args.description == NULL || args.arguments != NULL))
		return 1;

	if (args.mode == REPLACE && (args.name == NULL || args.file == NULL
	    || args.description != NULL || args.arguments != NULL))
		return 1;

	if (args.mode == DELETE && (args.name == NULL || args.file != NULL
	    || args.description != NULL || args.arguments != NULL || args.remove_file != -1))
		return 1;

	if (args.mode == HELP && (args.name != NULL || args.file != NULL
	    || args.description != NULL || args.arguments != NULL || args.remove_file != -1))
		return 1;

	if (args.mode == SEARCH && ((args.name == NULL && args.description == NULL)
	    || args.arguments != NULL || args.remove_file != -1))
		return 1;

	if (args.mode == LIST && (args.name != NULL || args.file != NULL
	    || args.description != NULL || args.arguments != NULL || args.remove_file != -1))
		return 1;

	if (args.mode == VERSION && (args.name != NULL || args.file != NULL
	    || args.description != NULL || args.arguments != NULL || args.remove_file != -1))
		return 1;

	if (args.mode == EXECUTE && (args.name == NULL || args.file != NULL
	    || args.description != NULL))
		return 1;

	if (args.mode == ECHO && (args.name == NULL || args.file != NULL
	    || args.description != NULL || args.arguments != NULL))
		return 1;

	return 0;
}

static int
init_sm(void)
{
	const char *homedir = getpwuid(getuid())->pw_dir;
	int script_path_size = strlen(homedir) + strlen(SCRIPT_DB_DIR) + 1;
	script_path = malloc(script_path_size);

	if (script_path == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	snprintf(script_path, script_path_size, "%s%s", homedir, SCRIPT_DB_DIR);

	if (access(script_path, F_OK) == -1)
		if (mkdir(script_path,  0755)) {
			fprintf(stderr, "Error creating directory!\n");
			return 1;
		}

	script_path_size += strlen(SCRIPT_DB_FILE);
	char *script_db_path = malloc(script_path_size);

	if (script_db_path == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	snprintf(script_db_path, script_path_size, "%s%s", script_path, SCRIPT_DB_FILE);
	int res = sqlite3_open(script_db_path, &db);
	free(script_db_path);

	if (res != SQLITE_OK) {
		fprintf(stderr, "Error opening script database: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	char *err = NULL;

	if (sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS " SCRIPT_TABLE " ("
	    "id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL, "
	    "description TEXT NOT NULL, CONSTRAINT name_unique UNIQUE (name));",
	    NULL, NULL, &err) != SQLITE_OK) {
		fprintf(stderr, "SQLite error: %s\n", err);
		sqlite3_free(err);
		return 1;
	}

	return 0;
}

static int
add_script(void)
{
	if (access(args.file, F_OK)) {
		fprintf(stderr, "File does not exist!\n");
		return 1;
	}

	sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	sqlite3_stmt *add_stmt = NULL;

	if (sqlite3_prepare_v2(db, "INSERT INTO " SCRIPT_TABLE " (name, description) VALUES (?, ?);",
	    -1, &add_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_bind_text(add_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
		sqlite3_finalize(add_stmt);
		fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_bind_text(add_stmt, 2, args.description, strlen(args.description), NULL) != SQLITE_OK) {
		sqlite3_finalize(add_stmt);
		fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_step(add_stmt) != SQLITE_DONE) {
		sqlite3_finalize(add_stmt);
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	sqlite3_finalize(add_stmt);
	sqlite3_stmt *find_id_stmt = NULL;

	if (sqlite3_prepare_v2(db, "SELECT max(id) FROM " SCRIPT_TABLE, -1, &find_id_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "SQLite prepare error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_step(find_id_stmt) != SQLITE_ROW) {
		sqlite3_finalize(find_id_stmt);
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int script_id = sqlite3_column_int(find_id_stmt, 0);
	sqlite3_finalize(find_id_stmt);
	int script_id_len = strlen(script_path) + GET_INT_SIZE(script_id) + 2;
	char *new_file = malloc(script_id_len);

	if (new_file == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	snprintf(new_file, script_id_len, "%s/%d", script_path, script_id);
	int status = (args.remove_file == -1 || args.remove_file == 0)
	    ? copy_file(args.file, new_file) : rename(args.file, new_file);
	status += make_executable(new_file);

	if (status) {
		sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
		fprintf(stderr, "An error occured while adding script file!\n");
		return 1;
	}

	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
	puts("Script added.");
	return 0;
}

static int
delete_script(void)
{
	sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
	sqlite3_stmt *delete_find_stmt = NULL;

	if (sqlite3_prepare_v2(db, "SELECT id FROM " SCRIPT_TABLE " WHERE name = ?;",
	    -1, &delete_find_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_bind_text(delete_find_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
		sqlite3_finalize(delete_find_stmt);
		fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_step(delete_find_stmt) != SQLITE_ROW) {
		sqlite3_finalize(delete_find_stmt);
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int script_id = sqlite3_column_int(delete_find_stmt, 0);
	sqlite3_finalize(delete_find_stmt);
	sqlite3_stmt *delete_stmt = NULL;

	if (sqlite3_prepare_v2(db, "DELETE FROM " SCRIPT_TABLE "  WHERE name = ?;",
	    -1, &delete_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_bind_text(delete_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
		sqlite3_finalize(delete_stmt);
		fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_step(delete_stmt) != SQLITE_DONE) {
		sqlite3_finalize(delete_stmt);
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	sqlite3_finalize(delete_stmt);
	int script_id_len = GET_INT_SIZE(script_id);
	char *script = malloc(strlen(script_path) + script_id_len + 2);

	if (script == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	snprintf(script, strlen(script_path) + script_id_len + 2, "%s/%d", script_path, script_id);

	if (access(script, F_OK) != -1)
		if (remove(script)) {
			sqlite3_exec(db, "ROLLBACK;", NULL, NULL, NULL);
			fprintf(stderr, "Failed to remove script file!\n");
			return 1;
		}
	sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);
	puts("Script deleted.");
	return 0;
}

static int
execute_script(void)
{
	sqlite3_stmt *execute_stmt = NULL;

	if (sqlite3_prepare_v2(db, "SELECT id FROM " SCRIPT_TABLE " WHERE name = ?;",
	    -1, &execute_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_bind_text(execute_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
		sqlite3_finalize(execute_stmt);
		fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int result = sqlite3_step(execute_stmt);

	if (result == SQLITE_DONE) {
		sqlite3_finalize(execute_stmt);
		fprintf(stderr, "Script not found!\n");
		return 1;
	}

	if (result != SQLITE_ROW) {
		sqlite3_finalize(execute_stmt);
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int script_id = sqlite3_column_int(execute_stmt, 0);
	sqlite3_finalize(execute_stmt);
	int script_len = GET_INT_SIZE(script_id) + strlen(script_path) + 2;
	char *script = malloc(script_len);

	if (script == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	snprintf(script, script_len, "%s/%d", script_path, script_id);
	char **exec_args = malloc((args.arg_size + 2) * sizeof(char *));

	if (exec_args == NULL) {
		free(script);
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	exec_args[0] = script;

	for (size_t i = 1; i <= args.arg_size; i++)
		exec_args[i] = args.arguments[i - 1];

	exec_args[args.arg_size + 1] = NULL;
	execvp(script, exec_args);
	free(script);
	fprintf(stderr, "Error running script!");
	return 1;
}

static int
replace_script(void)
{
	if (access(args.file, F_OK)) {
		fprintf(stderr, "File does not exist!\n");
		return 1;
	}

	const char *replace_find_sql = "SELECT id FROM " SCRIPT_TABLE " WHERE name = ?;";
	sqlite3_stmt *replace_find_stmt = NULL;

	if (sqlite3_prepare_v2(db, replace_find_sql, -1, &replace_find_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_bind_text(replace_find_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
		sqlite3_finalize(replace_find_stmt);
		fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int result = sqlite3_step(replace_find_stmt);

	if (result == SQLITE_DONE) {
		sqlite3_finalize(replace_find_stmt);
		fprintf(stderr, "Script not fount!\n");
		return 1;
	}

	if (result != SQLITE_ROW) {
		sqlite3_finalize(replace_find_stmt);
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int script_id = sqlite3_column_int(replace_find_stmt, 0);
	sqlite3_finalize(replace_find_stmt);
	int script_file_len = strlen(script_path) + GET_INT_SIZE(script_id) + 2;
	char *script_file = malloc(script_file_len);

	if (script_file == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	snprintf(script_file, script_file_len, "%s/%d", script_path, script_id);
	int res = copy_file(args.file, script_file);
	free(script_file);

	if (res) {
		fprintf(stderr, "Failed to replace file!\n");
		return 1;
	}

	if (args.remove_file == 1)
		if (remove(args.file))
			fprintf(stderr, "Failed to remove original file!\n");

	puts("File replaced.");
	return 0;
}

static int
search_script(void)
{
	sqlite3_stmt *search_stmt = NULL;

	if (args.name != NULL && args.description != NULL) {
		if (sqlite3_prepare_v2(db, "SELECT * FROM " SCRIPT_TABLE " WHERE instr(name, ?) > 0 AND instr(description, ?) > 0;",
		    -1, &search_stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "SQLite error!");
			return 1;
		}

		if (sqlite3_bind_text(search_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
			sqlite3_finalize(search_stmt);
			fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
			return 1;
		}

		if (sqlite3_bind_text(search_stmt, 2, args.description, strlen(args.description), NULL) != SQLITE_OK) {
			sqlite3_finalize(search_stmt);
			fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
			return 1;
		}
	} else if (args.name != NULL) {
		if (sqlite3_prepare_v2(db, "SELECT * FROM " SCRIPT_TABLE " WHERE instr(name, ?) > 0;", -1,
		    &search_stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "SQLite error!");
			return 1;
		}

		if (sqlite3_bind_text(search_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
			sqlite3_finalize(search_stmt);
			fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
			return 1;
		}
	} else { /* args.description != NULL */
		if (sqlite3_prepare_v2(db, "SELECT * FROM " SCRIPT_TABLE " WHERE instr(description, ?) > 0;",
		    -1, &search_stmt, NULL) != SQLITE_OK) {
			fprintf(stderr, "SQLite error!");
			return 1;
		}

		if (sqlite3_bind_text(search_stmt, 1, args.description, strlen(args.description), NULL) != SQLITE_OK) {
			sqlite3_finalize(search_stmt);
			fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
			return 1;
		}
	}

	int err = 0;
	pid_t pid;
	int p[2];

	if (isatty(STDOUT_FILENO) && args.page) {
		if (pipe(p)) {
			err = 1;
		} else if ((pid = fork()) == -1) {
			err = 1;
		} else if (pid == 0) {
			close(p[0]);
			dup2(p[1], STDOUT_FILENO);
			close(p[1]);
		} else {
			char *pager_args[] = { "less", NULL };
			close(p[1]);
			dup2(p[0], STDIN_FILENO);
			close(p[0]);
			execvp(pager_args[0], pager_args);
		}
	}

	if (!err)
		for (;;) {
			int result = sqlite3_step(search_stmt);

			if (result == SQLITE_ROW) {
				printf("id: %d\nname: %s\ndescription: %s\n\n", sqlite3_column_int(search_stmt, 0),
				sqlite3_column_text(search_stmt, 1), sqlite3_column_text(search_stmt, 2));
			} else if (result == SQLITE_DONE) {
				break;
			} else {
				sqlite3_finalize(search_stmt);
				fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
				err = 1;
				break;
			}
		}

	sqlite3_finalize(search_stmt);
	return err;
}

static int
echo_script(void)
{
	sqlite3_stmt *echo_stmt = NULL;

	if (sqlite3_prepare_v2(db, "SELECT id FROM " SCRIPT_TABLE " WHERE name = ?;",
	    -1, &echo_stmt, NULL) != SQLITE_OK) {
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	if (sqlite3_bind_text(echo_stmt, 1, args.name, strlen(args.name), NULL) != SQLITE_OK) {
		sqlite3_finalize(echo_stmt);
		fprintf(stderr, "SQLite bind error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int result = sqlite3_step(echo_stmt);

	if (result == SQLITE_DONE) {
		sqlite3_finalize(echo_stmt);
		fprintf(stderr, "Script not found!\n");
		return 1;
	}

	if (result != SQLITE_ROW) {
		sqlite3_finalize(echo_stmt);
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		return 1;
	}

	int script_id = sqlite3_column_int(echo_stmt, 0);
	sqlite3_finalize(echo_stmt);
	int script_len = GET_INT_SIZE(script_id) + strlen(script_path) + 2;
	char *script = malloc(script_len);

	if (script == NULL) {
		fprintf(stderr, "Error allocating memory!\n");
		return 2;
	}

	snprintf(script, script_len, "%s/%d", script_path, script_id);
	int err = 0;

	if (!isatty(STDOUT_FILENO) || !args.page) {
		if (print_file(script)) {
			fprintf(stderr, "Error opening file!\n");
			err = 1;
		}
	} else {
		pid_t pid;
		int p[2];

		if (pipe(p)) {
			fprintf(stderr, "Error opening pipe!\n");
			err = 1;
		} else {
			if ((pid = fork()) == -1) {
				fprintf(stderr, "Failed to fork!\n");
				err = 1;
			} else if (pid == 0) {
				close(p[0]);
				dup2(p[1], STDOUT_FILENO);
				close(p[1]);
				if (print_file(script)) {
					fprintf(stderr, "Error opening file!\n");
					err = 1;
				}
			} else {
				char *pager_args[] = { "less", NULL };
				close(p[1]);
				dup2(p[0], STDIN_FILENO);
				close(p[0]);
				execvp(pager_args[0], pager_args);
			}
		}
	}
	free(script);
	return err;
}

static int
list_script(void)
{
	int err = 0;
	pid_t pid;
	int p[2];

	if (isatty(STDOUT_FILENO) || args.page) {
		if (pipe(p)) {
			err = 1;
		} else if ((pid = fork()) == -1) {
			err = 1;
		} else if (pid == 0) {
			close(p[0]);
			dup2(p[1], STDOUT_FILENO);
			close(p[1]);
		} else {
			char *pager_args[] = { "less", NULL };
			close(p[1]);
			dup2(p[0], STDIN_FILENO);
			close(p[0]);
			execvp(pager_args[0], pager_args);
		}
	}

	if (sqlite3_exec(db, "SELECT * FROM " SCRIPT_TABLE ";", list_script_callback, 0, NULL)) {
		fprintf(stderr, "SQLite error: %s\n", sqlite3_errmsg(db));
		err = 1;
	}

	return err;
}

static int
list_script_callback(void *not_used, int argc, char **argv, char **column)
{
	for (size_t i = 0; i < argc; i++) {
		printf("%s: %s\n", column[i], argv[i]);
	}

	puts("");
	return 0;
}
