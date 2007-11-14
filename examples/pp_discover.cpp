#include "pp.h"
#include "drivers.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "utils.h"

using namespace std;

void
dump_scope(const pp_scope *scope, string indent = "")
{
	#if 0
	for (size_t i = 0; i < scope->n_datatypes(); i++) {
		cout << indent;
		cout << "datatype: "
		     << scope->datatype_name(i)
		     << endl;
	}
	#endif
	for (size_t i = 0; i < scope->n_dirents(); i++) {
		#if 0
		if (scope->dirent(i)->is_register()) {
			cout << indent;
			cout << "register "
			     << scope->dirent_name(i)
			     << ": (0x"
			     << std::hex
			     << pp_register_from_dirent(
			        scope->dirent(i))->read()
			     << ")"
			     << endl;
		}
		#endif
		if (scope->dirent(i)->is_field()) {
			cout << indent;
			cout << scope->dirent_name(i)
			     << ": "
			     << pp_field_from_dirent(
			        scope->dirent(i))->evaluate()
			     << " (0x"
			     << std::hex
			     << pp_field_from_dirent(
			        scope->dirent(i))->read()
			     << ")"
			     << endl;
		}
		if (scope->dirent(i)->is_scope()) {
			const pp_scope *sub;
			sub = pp_scope_from_dirent(scope->dirent(i));

			cout << indent;
			cout << scope->dirent_name(i);
			if (sub->binding()) {
				cout << " (@" << *sub->binding() << ")";
			}
			cout << " {"
			     << endl;

			dump_scope(sub, indent+"    ");
			cout << indent;
			cout << "}"
			     << endl;
		}
	}
}

int
main()
{
	pp_scope_ptr platform = NEW_PLATFORM();
	do_discovery(platform.get());
	dump_scope(platform.get());
	return 0;
}
