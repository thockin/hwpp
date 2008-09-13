#ifndef CMDLINE_H__
#define CMDLINE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* a command line option */
struct cmdline_opt {
	const char *short_name;
	const char *long_name;
	int has_arg;
	const char *arg_help;
	const char *help;
};

/*
 * Parse command line options.
 *
 * Scan argv for options which match a cmdline_opt, and call the callback
 * if found.  When this function returns successfully, argc and argv have
 * been updated to only represent unmatched commandline options, with
 * argv[0] being the prgram name, as invoked.
 *
 * Returns 0 on success.
 * Returns -1 on error.
 */
extern int
cmdline_parse(int *argc, const char **argv[], const struct cmdline_opt *opts,
              void (*callback)(const char *opt, const char *arg));

/* print the commandline help for each cmdline_opt */
extern void
cmdline_help(int which_out, const struct cmdline_opt *opts);
#define CMDLINE_STDOUT 1
#define CMDLINE_STDERR 2

extern const char *cmdline_progname;
extern int cmdline_argc;
extern const char * const *cmdline_argv;

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* CMDLINE_H__ */
