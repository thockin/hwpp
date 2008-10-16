#include "pp.h"
#include "printfxx.h"
#include "drivers.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pp_array.h"
#include "pp_alias.h"
#include "cmdline.h"

using namespace std;

cmdline_bool skip_regs = false;
cmdline_bool skip_fields = false;
cmdline_bool skip_scopes = false;
cmdline_bool skip_aliases = false;

static void
dump_field(const string &name, const pp_field_const_ptr &field);
static void
dump_register(const string &name, const pp_register_const_ptr &reg);
static void
dump_scope(const string &name, const pp_scope_const_ptr &scope);
static void
dump_array(const string &name, const pp_array_const_ptr &array);
static void
dump_alias(const string &name, const pp_alias_const_ptr &alias);

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
		string subname = sprintfxx("%s[%d]", name, i);
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
		} else if (array->array_type() == PP_DIRENT_ALIAS) {
			dump_alias(subname,
			    pp_alias_from_dirent(array->at(i)));
		}
	}
}

static void
dump_alias(const string &name, const pp_alias_const_ptr &alias)
{
	if (!skip_aliases) {
		cout << name << ": ->"
		     << std::hex
		     << alias->link_path()
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
		string subname = sprintfxx("%s/%s",name,scope->dirent_name(i));
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
		} else if (scope->dirent(i)->is_alias()) {
			dump_alias(subname,
			    pp_alias_from_dirent(scope->dirent(i)));
		}
	}
}

static void do_help(...);
static struct cmdline_opt pp_opts[] = {
	{
		"nr", "no-registers",
		CMDLINE_OPT_BOOL, &skip_regs,
		"", "don't print registers"
	},
	{
		"nf", "no-fields",
		CMDLINE_OPT_BOOL, &skip_fields,
		"", "don't print fields"
	},
	{
		"ns", "no-scopes",
		CMDLINE_OPT_BOOL, &skip_scopes,
		"", "don't print scopes"
	},
	{
		"na", "no-aliases",
		CMDLINE_OPT_BOOL, &skip_aliases,
		"", "don't print aliases"
	},
	{
		"h", "help",
		CMDLINE_OPT_CALLBACK, (void *)do_help,
		"", "produce this help message"
	},
	CMDLINE_OPT_END_OF_LIST
};

static void
usage(const char *bad_opt)
{
	ostream *out;

	if (bad_opt == NULL) {
		out = &cout;
	} else {
		out = &cerr;
		*out << "unknown argument: '" << bad_opt << "'" << std::endl;
	}

	*out << "usage: " << cmdline_progname << " [OPTIONS]" << std::endl;
	*out << std::endl;
	*out << "OPTIONS:" << std::endl;
	while (const char *help_str = cmdline_help(pp_opts)) {
		*out << "  " << help_str << std::endl;
	}
	*out << std::endl;
}

static void
do_help(...)
{
	usage(NULL);
	exit(EXIT_SUCCESS);
}

int
main(int argc, const char *argv[])
{
	cmdline_parse(&argc, &argv, pp_opts);
	if (argc != 1) {
		usage(argv[1]);
		exit(EXIT_FAILURE);
	}

	pp_scope_ptr root = pp_init();
	pp_do_discovery();
	dump_scope("", root);

	return 0;
}
