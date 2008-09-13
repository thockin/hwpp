#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "cmdline.h"

/* globals */
const char *cmdline_progname;
int cmdline_argc;
const char * const *cmdline_argv;

static int
found_opt(const struct cmdline_opt *opt, int *argc, const char **argv[])
{
	if (opt->type == CMDLINE_OPT_BOOL) {
		/* no arg */
		*(cmdline_bool *)opt->arg = 1;
	} else if (opt->type == CMDLINE_OPT_COUNTER) {
		/* no arg */
		*(cmdline_uint *)opt->arg += 1;
	} else if (opt->type == CMDLINE_OPT_STRING) {
		/* string arg */
		(*argc)--; (*argv)++;
		if (*argc < 1) {
			return CMDLINE_ERR_REQUIRES_ARG;
		}
		*(cmdline_string *)opt->arg = (*argv)[0];
	} else if (opt->type == CMDLINE_OPT_INT) {
		/* signed number arg */
		(*argc)--; (*argv)++;
		if (*argc < 1) {
			return CMDLINE_ERR_REQUIRES_ARG;
		}
		*(cmdline_int *)opt->arg = strtoll((*argv)[0], NULL, 0);
	} else if (opt->type == CMDLINE_OPT_UINT) {
		/* unsigned number arg */
		(*argc)--; (*argv)++;
		if (*argc < 1) {
			return CMDLINE_ERR_REQUIRES_ARG;
		}
		*(cmdline_uint *)opt->arg = strtoull((*argv)[0], NULL, 0);
	} else if (opt->type == CMDLINE_OPT_CALLBACK) {
		/* callback function arg */
		cmdline_callback callback = (cmdline_callback)opt->arg;
		callback(opt, argc, argv);
	} else {
		return CMDLINE_ERR_UNKNOWN_TYPE;
	}
	return 0;
}

/* parse command line options */
int
cmdline_parse(int *argc, const char **argv[], const struct cmdline_opt *opts)
{
	const char **new_argv;
	int new_argc;

	/* save progname, argc, and argv */
	cmdline_progname = strrchr((*argv)[0], '/');
	if (cmdline_progname) {
		cmdline_progname++;
	} else {
		cmdline_progname = (*argv)[0];
	}
	cmdline_argc = *argc;
	cmdline_argv = *argv;
	(*argc)--; (*argv)++;

	/* allocate a new argv for leftover args, argc is a safe size */
	new_argc = 0;
	new_argv = malloc(sizeof(char *) * (*argc));
	if (new_argv == NULL) {
		return -1;
	}
	new_argv[new_argc++] = cmdline_progname;

	/* for each option on the command line */
	while (*argc) {
		int i;
		int found = 0;
		const char *argp = &(*argv)[0][0];

		/* check each valid option */
		for (i = 0; opts[i].short_name != NULL; i++) {
			const struct cmdline_opt *optp = &opts[i];
			if (strcmp(argp, optp->short_name) == 0
			 || strcmp(argp, optp->long_name) == 0) {
				/* found a known option */
				found = 1;
			 	found_opt(&opts[i], argc, argv);
				break;
			}
		}
		if (!found) {
			new_argv[new_argc++] = argp;
		}
		(*argc)--; (*argv)++;
	}

	(*argc) = new_argc;
	(*argv) = new_argv;
	return 0;
}

void
cmdline_help(int which_out, const struct cmdline_opt *opts)
{
	FILE *out;
	size_t maxlen1, maxlen2, maxlen3;
	const struct cmdline_opt *opt;

	if (which_out == CMDLINE_STDOUT) {
		out = stdout;
	} else {
		out = stderr;
	}
	maxlen1 = maxlen2 = maxlen3 = 0;
	for (opt = opts; opt->short_name; opt++) {
		size_t size;

		size = strlen(opt->short_name);
		if (size > maxlen1) {
			maxlen1 = size;
		}
		size = strlen(opt->long_name);
		if (size > maxlen2) {
			maxlen2 = size;
		}
		size = strlen(opt->arg_name);
		if (size > maxlen3) {
			maxlen3 = size;
		}
	}

	for (opt = opts; opt->short_name; opt++) {
		fprintf(out, "  %-*s %-*s%s%*s",
		        maxlen1, opt->short_name,
		        maxlen2, opt->long_name,
		        (maxlen3 > 0) ? " " : "",
		        maxlen3, opt->arg_name);
		fprintf(out, "  %s\n", opt->help);
	}
}
