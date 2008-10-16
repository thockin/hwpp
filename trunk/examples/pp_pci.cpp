#include "pp.h"
#include "printfxx.h"
#include "drivers.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pp_array.h"

using namespace std;

static void
dump_field(const string &name, const pp_field_const_ptr &field,
           const string &indent="");
static void
dump_reg(const string &name, const pp_register_const_ptr &reg,
         const string &indent="");
static void
dump_scope(const string &name, const pp_scope_const_ptr &scope,
           const string &indent="");
static void
dump_array(const string &name, const pp_array_const_ptr &array,
           const string &indent="");

static void
dump_field(const string &name, const pp_field_const_ptr &field,
           const string &indent)
{
	cout << indent;
	cout << name << ": "
	     << field->evaluate()
	     << std::hex
	     << " (0x" << field->read() << ")"
	     << endl;
}

static void
dump_reg(const string &name, const pp_register_const_ptr &reg,
         const string &indent)
{
	cout << indent;
	cout << name << ": "
	     << std::hex
	     << "0x" << reg->read()
	     << endl;
}

static void
dump_array(const string &name, const pp_array_const_ptr &array,
           const string &indent)
{
	for (size_t i = 0; i < array->size(); i++) {
		string subname = sprintfxx("%s[%d]", name, i);
		if (array->array_type() == PP_DIRENT_FIELD) {
			const pp_field_const_ptr &field =
			    pp_field_from_dirent(array->at(i));
			dump_field(subname, field, indent);
		} else if (array->array_type() == PP_DIRENT_SCOPE) {
			const pp_scope_const_ptr &scope =
			    pp_scope_from_dirent(array->at(i));
			dump_scope(subname, scope, indent);
		} else if (array->array_type() == PP_DIRENT_ARRAY) {
			const pp_array_const_ptr &subarray =
			    pp_array_from_dirent(array->at(i));
			dump_array(subname, subarray, indent);
		}
	}
}

static void
dump_scope(const string &name, const pp_scope_const_ptr &scope,
           const string &indent)
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
	pp_scope_ptr root = pp_init();
	pp_do_discovery("pci");
	dump_scope("", root);
	return 0;
}
