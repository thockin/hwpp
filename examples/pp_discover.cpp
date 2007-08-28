#include "pp.h"
#include "drivers.h"
#include "pp_space.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "utils.h"

using namespace std;

void
dump_container(const pp_container *ctr, string indent = "")
{
	#if 0
	for (size_t i = 0; i < ctr->n_datatypes(); i++) {
		cout << indent;
		cout << "datatype: "
		     << ctr->datatype_name(i)
		     << endl;
	}
	#endif
	for (size_t i = 0; i < ctr->n_dirents(); i++) {
		#if 0
		if (ctr->dirent(i)->is_register()) {
			cout << indent;
			cout << "register "
			     << ctr->dirent_name(i)
			     << ": (0x"
			     << std::hex
			     << pp_register_from_dirent(
			        ctr->dirent(i))->read()
			     << ")"
			     << endl;
		}
		#endif
		if (ctr->dirent(i)->is_field()) {
			cout << indent;
			cout << ctr->dirent_name(i)
			     << ": "
			     << pp_field_from_dirent(
			        ctr->dirent(i))->evaluate()
			     << " (0x"
			     << std::hex
			     << pp_field_from_dirent(
			        ctr->dirent(i))->read()
			     << ")"
			     << endl;
		}
		if (ctr->dirent(i)->is_scope()
		 || ctr->dirent(i)->is_space()
		 || ctr->dirent(i)->is_device()) {
			cout << indent;
			cout << ctr->dirent_name(i)
			     << " {"
			     << endl;
			const pp_container *sub;
			
			if (ctr->dirent(i)->is_scope()) {
				sub = pp_scope_from_dirent(ctr->dirent(i));
			} else if (ctr->dirent(i)->is_space()) {
				sub = pp_space_from_dirent(ctr->dirent(i));
			} else if (ctr->dirent(i)->is_device()) {
				sub = pp_device_from_dirent(ctr->dirent(i));
			}
			dump_container(sub, indent+"    ");
			cout << indent;
			cout << "}"
			     << endl;
		}
	}
}

int
main()
{
	pp_platform_ptr platform = new_pp_platform();
	do_discovery(platform.get());
	dump_container(platform.get());
	return 0;
}
