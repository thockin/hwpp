#include "pp.h"
#include "drivers.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pp_array.h"
#include "cmdline.h"

using namespace std;

bool skip_regs = false;
bool skip_fields = false;
bool skip_scopes = false;

static void
dump_field(const string &name, const pp_field_const_ptr &field);
static void
dump_register(const string &name, const pp_register_const_ptr &reg);
static void
dump_scope(const string &name, const pp_scope_const_ptr &scope);
static void
dump_array(const string &name, const pp_array_const_ptr &array);

static void
dump_field(const string &name, const pp_field_const_ptr &field)
{
	if (!skip_fields) {
		cout << name << ": "
		     << field->evaluate()
		     << std::hex
		     << " (0x" << field->read() << ")"
		     << endl;
	}
}

static void
dump_register(const string &name, const pp_register_const_ptr &reg)
{
	if (!skip_regs) {
		cout << name << ": "
			<< std::hex
			<< "0x" << reg->read()
			<< endl;
	}
}

static void
dump_array(const string &name, const pp_array_const_ptr &array)
{
	for (size_t i = 0; i < array->size(); i++) {
		string subname = name + "[" + to_string(i) + "]";
		if (array->array_type() == PP_DIRENT_FIELD) {
			dump_field(subname,
			    pp_field_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_REGISTER) {
			dump_register(subname,
			    pp_register_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_SCOPE) {
			dump_scope(subname,
			    pp_scope_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_ARRAY) {
			dump_array(subname,
			    pp_array_from_dirent(array->at(i)));
		}
	}
}

static void
dump_scope(const string &name, const pp_scope_const_ptr &scope)
{
	if (!skip_scopes) {
		cout << name << "/";
		if (scope->is_bound()) {
			cout << " (@" << *scope->binding() << ")";
		}
		cout << endl;
	}

	for (size_t i = 0; i < scope->n_dirents(); i++) {
		string subname = name + "/" + scope->dirent_name(i);
		if (scope->dirent(i)->is_field()) {
			dump_field(subname,
			    pp_field_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_register()) {
			dump_register(subname,
			    pp_register_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_scope()) {
			dump_scope(subname,
			    pp_scope_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_array()) {
			dump_array(subname,
			    pp_array_from_dirent(scope->dirent(i)));
		}
	}
}

static struct cmdline_opt pp_opts[] = {
	{
		"-nr", "--no-registers",
		false, "",
		"don't print registers"
	},
	{
		"-nf", "--no-fields",
		false, "",
		"don't print fields"
	},
	{
		"-ns", "--no-scopes",
		false, "",
		"don't print scopes"
	},
	{
		"-h", "--help",
		false, "",
		"produce this help message"
	},
	{ NULL, NULL, false, NULL, NULL },
};

static void
usage(const char *bad_opt)
{
	ostream *out;
	int which_out;

	if (bad_opt == NULL) {
		out = &cout;
		which_out = CMDLINE_STDOUT;
	} else {
		out = &cerr;
		which_out = CMDLINE_STDERR;
		*out << "unknown option: '" << bad_opt << "'" << std::endl;
	}

	*out << "usage: " << cmdline_progname << " [OPTIONS]" << std::endl;
	*out << std::endl;
	*out << "OPTIONS:" << std::endl;

	cmdline_help(which_out, pp_opts);
}

static void
cmdline_callback(const char *opt, const char *arg)
{
	(void)arg;
	if (!strcmp(opt, "-nr") || !strcmp(opt, "--no-registers")) {
		skip_regs = true;
		return;
	}
	if (!strcmp(opt, "-nf") || !strcmp(opt, "--no-fields")) {
		skip_fields = true;
		return;
	}
	if (!strcmp(opt, "-ns") || !strcmp(opt, "--no-scopes")) {
		skip_scopes = true;
		return;
	}
	if (!strcmp(opt, "-h") || !strcmp(opt, "--help")) {
		usage(NULL);
		exit(EXIT_SUCCESS);
	}
}

int
main(int argc, const char *argv[])
{
	cmdline_parse(&argc, &argv, pp_opts, cmdline_callback);
	if (argc != 1) {
		usage(argv[1]);
		exit(EXIT_FAILURE);
	}

	pp_scope_ptr root = pp_init();
	pp_do_discovery();
	dump_scope("", root);

	return 0;
}
