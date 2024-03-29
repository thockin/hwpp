#include "language/environment.h"
#include <stdio.h>
#include <map>
#include "language/language.h"
#include "language/parsed_file.h"
#include "util/string_file.h"

namespace hwpp {
namespace language {

Environment::~Environment()
{
	ParsedFileMap::iterator it;
	for (it = m_parsed_files.begin(); it != m_parsed_files.end(); ++it) {
		delete it->second;
	}
}

const ParsedFile *
Environment::parse_file(const string &name)
{
	FILE *file = fopen(name.c_str(), "r");
	if (!file) {
		return NULL;
	}
	// RAII for file.
	boost::shared_ptr<FILE> file_ptr(file, fclose);

	return parse_file(name, file);
}

const ParsedFile *
Environment::parse_file(const string &name, FILE *file)
{
	// Parse the file.
	std::auto_ptr<ParsedFile> parsed_file_ptr(new ParsedFile());
	ParsedFile *current_file = parsed_file_ptr.get();
	m_parser.parse_file(name, file, current_file);

	// Save the results.
	m_parsed_files[name] = parsed_file_ptr.release();

	// Add it to the module map if it has a module name.
	string module = current_file->module();
	if (module != "") {
		m_modules[module] = current_file;
	}

	return m_parsed_files[name];
}

const ParsedFile *
Environment::parse_string(const string &name, const string &data)
{
	util::StringFile sf;

	if (!sf.open(data)) {
		WARN("could not create StringFile for data");
		return NULL;
	}

	return parse_file(name, sf.get_read_file());
}

int
Environment::validate(const ValidateOptions &flags)
{
	int warns = 0;

	ParsedFileMap::iterator it;
	for (it = m_parsed_files.begin(); it != m_parsed_files.end(); ++it) {
		ParsedFile *file = it->second;
		for (size_t i = 0; i < file->n_statements(); i++) {
			warns += file->statement(i)->validate_once(flags, this);
		}
	}

	return warns;
}

syntax::Definition *
Environment::lookup_symbol(const string &name)
{
	syntax::Definition * const *p = m_symtab.lookup(name);
	if (p == NULL) {
		return NULL;
	}
	return *p;
}

const ParsedFile *
Environment::lookup_module(const string &name)
{
	ModuleMap::iterator it = m_modules.find(name);
	if (it != m_modules.end()) {
		return it->second;
	}
	return NULL;
}

}  // namespace language
}  // namespace hwpp

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
