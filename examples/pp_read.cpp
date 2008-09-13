// This tool takes a list of PP paths (on the commandline or stdin) and
// reads them.  If a path is a scope or array, it will recurse.
#include "pp.h"
#include "drivers.h"
#include "pp_field.h"
#include "pp_register.h"
#include "pp_scope.h"
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
		} else {
			cerr << subname << "unknown dirent type: "
			     << scope->dirent(i)->dirent_type() << endl;
		}
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
		} else {
			cerr << name << ": unknown array type: "
			     << array->array_type() << endl;
			return;
		}
	}
}

static void
dump_dirent(pp_scope_ptr &root, string path)
{
	// special-case for "/"
	if (path == "/") {
		path = "";
	}

	const pp_dirent_const_ptr &de = root->lookup_dirent(path);
	if (de == NULL) {
		cerr << path << ": path not found" << endl;
	} else if (de->is_field()) {
		dump_field(path, pp_field_from_dirent(de));
	} else if (de->is_register()) {
		dump_register(path, pp_register_from_dirent(de));
	} else if (de->is_scope()) {
		dump_scope(path, pp_scope_from_dirent(de));
	} else if (de->is_array()) {
		dump_array(path, pp_array_from_dirent(de));
	} else {
		cerr << path << ": unknown dirent type: "
		     << de->dirent_type() << endl;
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
usage()
{
	cout << "usage: " << cmdline_progname << " [OPTIONS]" << std::endl;
	cout << std::endl;
	cout << "OPTIONS:" << std::endl;
	cmdline_help(CMDLINE_STDOUT, pp_opts);
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
		usage();
		exit(EXIT_SUCCESS);
	}
}

int
main(int argc, const char *argv[])
{
	cmdline_parse(&argc, &argv, pp_opts, cmdline_callback);

	pp_scope_ptr root = pp_init();
	pp_do_discovery();

	if (argc == 1) {
		string path;
		while (cin >> path) {
			dump_dirent(root, path);
		}
	} else {
		for (int i = 1; i < argc; i++) {
			dump_dirent(root, argv[i]);
		}
	}

	return 0;
}
