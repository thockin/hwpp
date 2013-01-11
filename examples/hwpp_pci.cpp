#include "hwpp.h"
#include "util/printfxx.h"
#include "drivers.h"
#include "device_init.h"
#include "register.h"
#include "datatype_types.h"
#include "field_types.h"
#include "array.h"

using namespace std;

static void
dump_field(const string &name, const hwpp::ConstFieldPtr &field,
           const string &indent="");
static void
dump_reg(const string &name, const hwpp::ConstRegisterPtr &reg,
         const string &indent="");
static void
dump_scope(const string &name, const hwpp::ConstScopePtr &scope,
           const string &indent="");
static void
dump_array(const string &name, const hwpp::ConstArrayPtr &array,
           const string &indent="");

static void
dump_field(const string &name, const hwpp::ConstFieldPtr &field,
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
dump_reg(const string &name, const hwpp::ConstRegisterPtr &reg,
         const string &indent)
{
	cout << indent;
	cout << name << ": "
	     << std::hex
	     << "0x" << reg->read()
	     << endl;
}

static void
dump_array(const string &name, const hwpp::ConstArrayPtr &array,
           const string &indent)
{
	for (size_t i = 0; i < array->size(); i++) {
		string subname = sprintfxx("%s[%d]", name, i);
		if (array->array_type() == hwpp::DIRENT_TYPE_FIELD) {
			const hwpp::ConstFieldPtr &field =
			    hwpp::field_from_dirent(array->at(i));
			dump_field(subname, field, indent);
		} else if (array->array_type() == hwpp::DIRENT_TYPE_SCOPE) {
			const hwpp::ConstScopePtr &scope =
			    hwpp::scope_from_dirent(array->at(i));
			dump_scope(subname, scope, indent);
		} else if (array->array_type() == hwpp::DIRENT_TYPE_ARRAY) {
			const hwpp::ConstArrayPtr &subarray =
			    hwpp::array_from_dirent(array->at(i));
			dump_array(subname, subarray, indent);
		}
	}
}

static void
dump_scope(const string &name, const hwpp::ConstScopePtr &scope,
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
			    hwpp::register_from_dirent(scope->dirent(i)),
			    indent+"    ");
		} else if (scope->dirent(i)->is_field()) {
			dump_field(scope->dirent_name(i),
			    hwpp::field_from_dirent(scope->dirent(i)),
			    indent+"    ");
		} else if (scope->dirent(i)->is_scope()) {
			dump_scope(scope->dirent_name(i),
			    hwpp::scope_from_dirent(scope->dirent(i)),
			    indent+"    ");
		} else if (scope->dirent(i)->is_array()) {
			dump_array(scope->dirent_name(i),
			    hwpp::array_from_dirent(scope->dirent(i)),
			    indent+"    ");
		}
	}

	cout << indent << "}" << endl;
}

int
main()
{
	hwpp::ScopePtr root = hwpp::initialize_device_tree();
	hwpp::do_discovery("pci");
	dump_scope("", root);
	return 0;
}
