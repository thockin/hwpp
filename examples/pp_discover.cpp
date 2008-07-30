#include "pp.h"
#include "drivers.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pp_array.h"

using namespace std;

void
dump_field(const string &name, const pp_field_const_ptr &field);
void
dump_scope(const string &name, const pp_scope_const_ptr &scope);
void
dump_array(const string &name, const pp_array_const_ptr &array);

void
dump_field(const string &name, const pp_field_const_ptr &field)
{
	cout << name << ": "
	     << field->evaluate()
	     << std::hex
	     << " (0x" << field->read() << ")"
	     << endl;
}

void
dump_array(const string &name, const pp_array_const_ptr &array)
{
	for (size_t i = 0; i < array->size(); i++) {
		string subname = name + "[" + to_string(i) + "]";
		if (array->array_type() == PP_DIRENT_FIELD) {
			dump_field(subname,
			    pp_field_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_SCOPE) {
			dump_scope(subname,
			    pp_scope_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_ARRAY) {
			dump_array(subname,
			    pp_array_from_dirent(array->at(i)));
		}
	}
}

void
dump_scope(const string &name, const pp_scope_const_ptr &scope)
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
		} else if (scope->dirent(i)->is_scope()) {
			dump_scope(subname,
			    pp_scope_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_array()) {
			dump_array(subname,
			    pp_array_from_dirent(scope->dirent(i)));
		}
	}
}

int
main()
{
	pp_scope_ptr root = pp_init();
	pp_do_discovery();
	dump_scope("", root);
	return 0;
}
