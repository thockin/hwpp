#include "pp.h"
#include "printfxx.h"
#include <signal.h>
#include "drivers.h"
#include "pp_field.h"
#include "pp_register.h"
#include "pp_scope.h"
#include "pp_array.h"
#include "sockets.h"

using namespace std;

string
dump_field(const string &name, const pp_field_const_ptr &field);
string
dump_register(const string &name, const pp_register_const_ptr &reg);
string
dump_scope(const string &name, const pp_scope_const_ptr &scope);
string
dump_array(const string &name, const pp_array_const_ptr &array);

string
dump_field(const string &name, const pp_field_const_ptr &field)
{
	stringstream s;
	s << name << ": "
	  << field->evaluate()
	  << std::hex
	  << " (0x" << field->read() << ")"
	  << endl;
	return s.str();
}

string
dump_register(const string &name, const pp_register_const_ptr &reg)
{
	stringstream s;
	s << name << ": "
	  << std::hex
	  << "0x" << reg->read()
	  << endl;
	return s.str();
}

string
dump_scope(const string &name, const pp_scope_const_ptr &scope)
{
	stringstream s;
	s << name << "/";
	if (scope->is_bound()) {
		s << " (@" << *scope->binding() << ")";
	}
	s << endl;

	for (size_t i = 0; i < scope->n_dirents(); i++) {
		string subname = sprintfxx("%s/%s",name,scope->dirent_name(i));
		if (scope->dirent(i)->is_field()) {
			s << dump_field(subname,
			    pp_field_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_register()) {
			s << dump_register(subname,
			    pp_register_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_scope()) {
			s << dump_scope(subname,
			    pp_scope_from_dirent(scope->dirent(i)));
		} else if (scope->dirent(i)->is_array()) {
			s << dump_array(subname,
			    pp_array_from_dirent(scope->dirent(i)));
		} else {
			s << subname << "unknown dirent type: "
			  << scope->dirent(i)->dirent_type() << endl;
		}
	}

	return s.str();
}

string
dump_array(const string &name, const pp_array_const_ptr &array)
{
	stringstream s;
	for (size_t i = 0; i < array->size(); i++) {
		string subname = sprintfxx("%s[%d]", name, i);
		if (array->array_type() == PP_DIRENT_FIELD) {
			s << dump_field(subname,
			    pp_field_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_REGISTER) {
			s << dump_register(subname,
			    pp_register_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_SCOPE) {
			s << dump_scope(subname,
			    pp_scope_from_dirent(array->at(i)));
		} else if (array->array_type() == PP_DIRENT_ARRAY) {
			s << dump_array(subname,
			    pp_array_from_dirent(array->at(i)));
		} else {
			s << name << ": unknown array type: "
			  << array->array_type() << endl;
			return s.str();
		}
	}

	return s.str();
}

string
dump_dirent(pp_scope_ptr &root, const string &path)
{
	stringstream s;
	const pp_dirent_const_ptr &de = root->lookup_dirent(path);
	if (de == NULL) {
		s << path << ": path not found" << endl;
	} else if (de->is_field()) {
		s << dump_field(path, pp_field_from_dirent(de));
	} else if (de->is_register()) {
		s << dump_register(path, pp_register_from_dirent(de));
	} else if (de->is_scope()) {
		s << dump_scope(path, pp_scope_from_dirent(de));
	} else if (de->is_array()) {
		s << dump_array(path, pp_array_from_dirent(de));
	} else {
		s << path << ": unknown dirent type: "
		  << de->dirent_type() << endl;
	}
	return s.str();
}

void
usage(ostream &out, const char *progname)
{
	out << "usage: " << progname << " socketpath" << endl;
}

void exit_handler(int sig) {
	(void)sig;
	cout << "Terminating server..." << endl;
	exit(EXIT_SUCCESS);
}

int
main(int argc, const char *argv[])
{
	if (argc > 1 && string(argv[1]) == "-h") {
		usage(cout, argv[0]);
		return EXIT_SUCCESS;
	}

	signal(SIGINT, exit_handler);
	signal(SIGTERM, exit_handler);
	signal(SIGQUIT, exit_handler);

	pp_scope_ptr root = pp_init();
	pp_do_discovery();

	if (argc == 2) {
		string socketpath(argv[1]);
		static unix_socket::server svr(socketpath);
		while (true) {
			// Handle new connections forever
			unix_socket::socket s = svr.accept();
			while (true) {
				string path = s.recv_line();
				if (!s.is_connected()) {
					break;
				}
				s.send(dump_dirent(root, path));
				s.send("\n");
			}
		}
	} else {
		usage(cout, argv[0]);
		return EXIT_SUCCESS;
	}

	return 0;
}
