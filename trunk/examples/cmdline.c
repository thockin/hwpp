#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "cmdline.h"

/* globals */
const char *cmdline_progname;
int cmdline_argc;
const char * const *cmdline_argv;

/* parse command line options */
int
cmdline_parse(int *argc, const char **argv[], const struct cmdline_opt *opts,
              void (*callback)(const char *opt, const char *arg))
{
	const char **new_argv;
	int new_argc;

	/* save progname, argc, and argv */
	cmdline_progname = (*argv)[0];
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
				const char *arg = NULL;
				found = 1;
				if (optp->has_arg) {
					(*argc)--; (*argv)++;
					arg = *(*argv);
				}
				/* call the callback */
				callback(argp, arg);
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
		size = strlen(opt->arg_help);
		if (size > maxlen3) {
			maxlen3 = size;
		}
	}

	for (opt = opts; opt->short_name; opt++) {
		fprintf(out, "  %-*s %-*s%s%*s",
		        maxlen1, opt->short_name,
		        maxlen2, opt->long_name,
		        (maxlen3 > 0) ? " " : "",
		        maxlen3, opt->arg_help);
		fprintf(out, "  %s\n", opt->help);
	}
	fprintf(out, "\n");
}
