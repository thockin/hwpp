#include "pp.h"
#include "drivers.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pp_array.h"
#include "pp_platform.h"

using namespace std;

void
dump_field(const string &name, const pp_field *field, const string &indent="");
void
dump_reg(const string &name, const pp_register *reg, const string &indent="");
void
dump_scope(const string &name, const pp_scope *scope, const string &indent="");
void
dump_array(const string &name, const pp_array *array, const string &indent="");

void
dump_field(const string &name, const pp_field *field, const string &indent)
{
	cout << indent;
	cout << name << ": "
	     << field->evaluate()
	     << std::hex
	     << " (0x" << field->read() << ")"
	     << endl;
}

void
dump_reg(const string &name, const pp_register *reg, const string &indent)
{
	cout << indent;
	cout << name << ": "
	     << std::hex
	     << "0x" << reg->read()
	     << endl;
}

void
dump_array(const string &name, const pp_array *array, const string &indent)
{
	for (size_t i = 0; i < array->size(); i++) {
		string subname = name + "[" + to_string(i) + "]";
		if (array->array_type() == PP_DIRENT_FIELD) {
			const pp_field *field;
			field = pp_field_from_dirent(array->at(i));
			dump_field(subname, field, indent);
		} else if (array->array_type() == PP_DIRENT_SCOPE) {
			const pp_scope *scope;
			scope = pp_scope_from_dirent(array->at(i));
			dump_scope(subname, scope, indent);
		} else if (array->array_type() == PP_DIRENT_ARRAY) {
			const pp_array *subarray;
			subarray = pp_array_from_dirent(array->at(i));
			dump_array(subname, subarray, indent);
		}
	}
}

void
dump_scope(const string &name, const pp_scope *scope, const string &indent)
{
	cout << indent << name;
	if (scope->is_bound()) {
		cout << " (@" << *scope->binding() << ")";
	}
	cout << " {" << endl;

	for (size_t i = 0; i < scope->n_dirents(); i++) {
		if (scope->dirent(i)->is_register()) {
			dump_reg(scope->dirent_name(i),
			    pp_register_from_dirent(scope->dirent(i)),
			    indent+"    ");
		} else if (scope->dirent(i)->is_field()) {
			dump_field(scope->dirent_name(i),
			    pp_field_from_dirent(scope->dirent(i)),
			    indent+"    ");
		} else if (scope->dirent(i)->is_scope()) {
			dump_scope(scope->dirent_name(i),
			    pp_scope_from_dirent(scope->dirent(i)),
			    indent+"    ");
		} else if (scope->dirent(i)->is_array()) {
			dump_array(scope->dirent_name(i),
			    pp_array_from_dirent(scope->dirent(i)),
			    indent+"    ");
		}
	}

	cout << indent << "}" << endl;
}

int
main()
{
	pp_platform platform;
	const pp_driver *driver = platform.find_driver("pci");
	driver->discover(platform.scope());
	dump_scope("", platform.scope());
	return 0;
}
