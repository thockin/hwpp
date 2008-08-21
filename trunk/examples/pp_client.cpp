#include <string>
#include <iostream>
#include <sstream>
#include "sockets.h"

using namespace std;

void
usage(ostream &out, const char *progname)
{
	out << "usage: " << progname << " socketpath [pp_paths]" << endl;
}

int
main(int argc, const char *argv[])
{
	if ((argc > 1 && string(argv[1]) == "-h") ||
	    argc == 1) {
		usage(cout, argv[0]);
		return EXIT_SUCCESS;
	}

	string socketpath(argv[1]);
	unix_socket::socket s(socketpath);

	if (argc == 2) {
		string str;
		while (cin >> str) {
			s.send(str + "\n");
			while ((str = s.recv_line()) != "") {
				cout << str << endl;
			}
		}
	} else {
		for (int i = 2; i < argc; i++) {
			string str(argv[i]);
			s.send(str + "\n");
			while ((str = s.recv_line()) != "") {
				cout << str << endl;
			}
		}
	}

	if (!s.is_connected()) {
		cout << endl << "Server terminated connection." << endl;
	}

	return EXIT_SUCCESS;
}
