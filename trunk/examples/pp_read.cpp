// This tool takes a list of PP paths (on the commandline or stdin) and
// reads them.  If a path is a scope or array, it will recurse.
#include "pp.h"
#include "drivers.h"
#include "pp_field.h"
#include "pp_register.h"
#include "pp_scope.h"
#include "pp_array.h"

using namespace std;

void
dump_field(const string &name, const pp_field *field);
void
dump_register(const string &name, const pp_scope *scope);
void
dump_scope(const string &name, const pp_scope *scope);
void
dump_array(const string &name, const pp_array *array);

void
dump_field(const string &name, const pp_field *field)
{
	cout << name << ": "
	     << field->evaluate()
	     << std::hex
	     << " (0x" << field->read() << ")"
	     << endl;
}

void
dump_register(const string &name, const pp_register *reg)
{
	cout << name << ": "
	     << std::hex
	     << "0x" << reg->read()
	     << endl;
}

void
dump_scope(const string &name, const pp_scope *scope)
{
	cout << name << "/";
	if (scope->is_bound()) {
		cout << " (@" << *scope->binding() << ")";
	}
	cout << endl;

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

void
dump_array(const string &name, const pp_array *array)
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

void
dump_dirent(pp_scope *root, const string &path)
{
	const pp_dirent *de = NULL;
	try {
		de = root->lookup_dirent(path);
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
	} catch (exception &e) {
		cerr << path << ": " << e.what() << endl;
	}
}

void
usage(ostream &out, const char *progname)
{
	out << "usage: " << progname << " <paths>" << endl;
}

int
main(int argc, const char *argv[])
{
	if (argc > 1 && string(argv[0]) == "-h") {
		usage(cout, argv[0]);
		return EXIT_SUCCESS;
	}

	pp_scope *root = pp_init();
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
