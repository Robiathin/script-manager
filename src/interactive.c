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
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "interactive.h"

int
parse_args(arg_options_t *arg_options, int argc, char **argv[])
{
	arg_options_t args;
	int error;
	char option;

	memset(&args, 0, sizeof(arg_options_t));
	args.mode = NOT_SET;
	args.page = 1;

	error = 0;

	while ((option = getopt(argc, *argv, "a:d:e:E:hkKlpPr:svV:A:n:f:D:C")) != -1) {
		switch (option) {
		case 'a': /* ADD */
			if (args.mode == NOT_SET && args.name == NULL) {
				args.mode = ADD;
				args.name = optarg;
			} else
				error = 1;

			break;
		case 'd': /* DELETE */
			if (args.mode == NOT_SET && args.name == NULL) {
				args.mode = DELETE;
				args.name = optarg;
			} else
				error = 1;

			break;
		case 'e': /* EXECUTE */
			if (args.mode == NOT_SET && args.name == NULL) {
				args.mode = EXECUTE;
				args.name = optarg;
			} else
				error = 1;

			break;
		case 'E': /* ECHO */
			if (args.mode == NOT_SET && args.name == NULL) {
				args.mode = ECHO;
				args.name = optarg;
			} else
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
		case 'l': /* LIST */
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
			if (args.mode == NOT_SET && args.name == NULL) {
				args.mode = REPLACE;
				args.name = optarg;
			} else {
				error = 1;
			}

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

			if (args.arguments == NULL)
				error = 2;
			else {
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
		case 'V': /* EDIT */
			if (args.mode == NOT_SET && args.name == NULL) {
				args.mode = EDIT;
				args.name = optarg;
			} else
				error = 1;

			break;
#ifdef WITH_AUTOCOMPLETE
		case 'C': /* COMPLETE */
			if (args.mode == NOT_SET)
				args.mode = COMPLETE;
			else
				error = 1;

			break;
#endif /* WITH_AUTOCOMPLETE */
		default:
			error = 1;
		}

		if (error)
			return (error);
	}

	if (validate_args(&args))
		return (1);

	*arg_options = args;

	return (0);
}

int
validate_args(arg_options_t *args)
{
	if (args->mode == NOT_SET)
		return (1);

	if (args->mode == ADD && (args->name == NULL || args->file == NULL
	    || args->description == NULL || args->arguments != NULL))
		return (1);

	if (args->mode == REPLACE && (args->name == NULL || args->file == NULL
	    || args->description != NULL || args->arguments != NULL))
		return (1);

	if (args->mode == DELETE && (args->name == NULL || args->file != NULL
	     || args->description != NULL || args->arguments != NULL))
		return (1);

	if (args->mode == HELP && (args->name != NULL || args->file != NULL
	    || args->description != NULL || args->arguments != NULL))
		return (1);

	if (args->mode == SEARCH && ((args->name == NULL && args->description == NULL)
	    || args->arguments != NULL))
		return (1);

	if (args->mode == LIST && (args->name != NULL || args->file != NULL
	   || args->description != NULL || args->arguments != NULL))
		return (1);

	if (args->mode == VERSION && (args->name != NULL || args->file != NULL
	    || args->description != NULL || args->arguments != NULL))
		return (1);

	if (args->mode == EXECUTE && (args->name == NULL || args->file != NULL
	    || args->description != NULL))
		return (1);

	if (args->mode == ECHO && (args->name == NULL || args->file != NULL
	    || args->description != NULL || args->arguments != NULL))
		return (1);

	if (args->mode == EDIT && (args->name == NULL || args->file != NULL
	    || args->description != NULL || args->arguments != NULL))
		return (1);

#ifdef WITH_AUTOCOMPLETE
	if (args->mode == COMPLETE && (args->name != NULL || args->file != NULL
	    || args->description != NULL || args->arguments != NULL))
		return (1);
#endif /* WITH_AUTOCOMPLETE */

	return (0);
}

#ifndef NO_PAGE

int
do_page(void)
{
        int err;

        err = 0;


        pid_t pid;
        int p[2];
        char *pager_args[2];

        if (isatty(STDOUT_FILENO)) {
                if (pipe(p)) {
                        fprintf(stderr, "Error opening pipe!\n");
                        err = 1;
                } else if ((pid = fork()) == -1) {
                        fprintf(stderr, "Error forking!\n");
                        err = 1;
                } else if (pid == 0) {
                        close(p[0]);
                        dup2(p[1], STDOUT_FILENO);
                        close(p[1]);
                } else {
                        pager_args[0] = check_env(SM_PAGER_ENV, "PAGER", SM_DEFAULT_PAGER);
                        pager_args[1] = NULL;

                        close(p[1]);
                        dup2(p[0], STDIN_FILENO);
                        close(p[0]);

                        execvp(pager_args[0], pager_args);

                        fprintf(stderr, "Failed to execute pager!\n");
                        exit(1);
                }
        }

	return (err);
}

#endif /* NO_PAGE */

char *
check_env(char *primary, char *secondary, char *def)
{
        char *res;

        res = getenv(primary);

        if (!res)
                res = getenv(secondary);

        return (res ? res : def);
}
