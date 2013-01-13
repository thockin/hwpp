#include "hwpp.h"
#include "util/printfxx.h"
#include "drivers.h"
#include "device_init.h"
#include "register.h"
#include "datatype_types.h"
#include "field_types.h"
#include "array.h"
#include "alias.h"
#include "cmdline.h"

using namespace std;

cmdline_bool skip_regs = false;
cmdline_bool skip_fields = false;
cmdline_bool skip_scopes = false;
cmdline_bool skip_aliases = false;

static void
dump_field(const string &name, const hwpp::ConstFieldPtr &field);
static void
dump_register(const string &name, const hwpp::ConstRegisterPtr &reg);
static void
dump_scope(const string &name, const hwpp::ConstScopePtr &scope);
static void
dump_array(const string &name, const hwpp::ConstArrayPtr &array);
static void
dump_alias(const string &name, const hwpp::ConstAliasPtr &alias);

static void
dump_field(const string &name, const hwpp::ConstFieldPtr &field)
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
dump_register(const string &name, const hwpp::ConstRegisterPtr &reg)
{
	if (!skip_regs) {
		cout << name << ": "
			<< std::hex
			<< "0x" << reg->read()
			<< endl;
	}
}

static void
dump_array(const string &name, const hwpp::ConstArrayPtr &array)
{
	for (size_t i = 0; i < array->size(); i++) {
		string subname = sprintfxx("%s[%d]", name, i);
		if (array->array_type() == hwpp::DIRENT_TYPE_FIELD) {
			dump_field(subname,
			    hwpp::field_from_dirent(array->at(i)));
		} else if (array->array_type() == hwpp::DIRENT_TYPE_REGISTER) {
			dump_register(subname,
			    hwpp::register_from_dirent(array->at(i)));
		} else if (array->array_type() == hwpp::DIRENT_TYPE_SCOPE) {
			dump_scope(subname,
			    hwpp::scope_from_dirent(array->at(i)));
		} else if (array->array_type() == hwpp::DIRENT_TYPE_ARRAY) {
			dump_array(subname,
			    hwpp::array_from_dirent(array->at(i)));
		} else if (array->array_type() == hwpp::DIRENT_TYPE_ALIAS) {
			dump_alias(subname,
			    hwpp::alias_from_dirent(array->at(i)));
		}
	}
}

static void
dump_alias(const string &name, const hwpp::ConstAliasPtr &alias)
{
	if (!skip_aliases) {
		cout << name << ": ->"
		     << std::hex
		     << alias->link_path()
		     << endl;
	}
}
 
static void
dump_scope(const string &name, const hwpp::ConstScopePtr &scope)
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
			    hwpp::field_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_register()) {
			dump_register(subname,
			    hwpp::register_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_scope()) {
			dump_scope(subname,
			    hwpp::scope_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_array()) {
			dump_array(subname,
			    hwpp::array_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_alias()) {
			dump_alias(subname,
			    hwpp::alias_from_dirent(scope->dirent(i)));
		}
	}
}

static void do_help(...);
static struct cmdline_opt hwpp_opts[] = {
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
	while (const char *help_str = cmdline_help(hwpp_opts)) {
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
	cmdline_parse(&argc, &argv, hwpp_opts);
	if (argc != 1) {
		usage(argv[1]);
		exit(EXIT_FAILURE);
	}

	hwpp::ScopePtr root = hwpp::initialize_device_tree();
	hwpp::do_discovery();
	dump_scope("", root);

	return 0;
}
