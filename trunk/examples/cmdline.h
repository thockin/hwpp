#ifndef CMDLINE_H__
#define CMDLINE_H__

/* a command line option */
struct cmdline_opt {
	const char *short_name;
	const char *long_name;
	int has_arg;
	const char *arg_help;
	const char *help;
};

/* parse command line options */
extern int
cmdline_parse(int *argc, const char **argv[], const struct cmdline_opt *opts,
              void (*callback)(const char *opt, const char *arg));
extern void
cmdline_help(int which_out, const struct cmdline_opt *opts);
#define CMDLINE_STDOUT 1
#define CMDLINE_STDERR 2

extern const char *cmdline_progname;
extern int cmdline_argc;
extern const char * const *cmdline_argv;

#endif /* CMDLINE_H__ */
