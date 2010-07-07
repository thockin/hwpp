#ifndef PP_LANGUAGE_ENVIRONMENT_H__
#define PP_LANGUAGE_ENVIRONMENT_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include <stdio.h>
#include <ostream>
#include <map>
#include "language/language.h"
#include "util/symbol_table.h"

namespace pp {
namespace language {

// Forward declarations.
class ParsedFile;
namespace syntax {
	class Definition;
	class Identifier;
}

class ValidateOptions {
 private:
	enum Severity {
		SEVERITY_SILENT,
		SEVERITY_WARN,
		SEVERITY_ERROR,
	};

 public:
	ValidateOptions()
	    : m_warnings_stream(&std::cerr),
	      m_unresolved_symbols(SEVERITY_ERROR)
	{
	}

	ValidateOptions &
	set_warnings_stream(std::ostream &stream)
	{
		m_warnings_stream = &stream;
		return *this;
	}
	std::ostream &
	warnings_stream() const
	{
		return *m_warnings_stream;
	}

	ValidateOptions &
	set_warn_on_unresolved_symbols()
	{
		m_unresolved_symbols = SEVERITY_WARN;
		return *this;
	}
	ValidateOptions &
	set_error_on_unresolved_symbols()
	{
		m_unresolved_symbols = SEVERITY_ERROR;
		return *this;
	}
	bool
	warn_on_unresolved_symbols() const
	{
		return m_unresolved_symbols == SEVERITY_WARN;
	}
	bool
	error_on_unresolved_symbols() const
	{
		return m_unresolved_symbols == SEVERITY_ERROR;
	}

 private:
	std::ostream *m_warnings_stream;
	Severity m_unresolved_symbols;
};

class Environment {
	typedef std::map<string, ParsedFile*> ParsedFileMap;
	typedef std::map<string, ParsedFile*> ModuleMap;
 public:
	Environment()
	{
	}
	~Environment();

	// Open and parse a file name.  The caller owns the returned pointer.
	// Returns NULL on failure.
	const ParsedFile *
	parse_file(const string &name);

	// Validate all parsed files.  Returns the number of warnings
	// generated.
	int
	validate(const ValidateOptions &flags);

	// Get the default validate options.
	ValidateOptions
	default_validate_options()
	{
		return ValidateOptions();
	}

	// Create a new nested scope for symbols.  This is used while
	// validating parsed files to do symbol resolution.
	void
	new_symbol_scope()
	{
		m_symtab.push_context();
	}

	// Add a symbol to the current nested scope.  This is used while
	// validating parsed files to do symbol resolution.
	bool
	add_symbol(const string &name, syntax::Definition *definition)
	{
		return m_symtab.add_unique(name,definition);
	}

	// Terminate the current nested scope for symbols.  This is used while
	// validating parsed files to do symbol resolution.
	void
	end_symbol_scope()
	{
		m_symtab.pop_context();
	}

	// Look up a symbol by name in the current nested scope hierarchy.
	// This is used while validating parsed files to do symbol resolution.
	syntax::Definition *
	lookup_symbol(const string &name);

	const ParsedFile *
	lookup_module(const string &name);

 private:
	Parser m_parser;
	util::SymbolTable<string, syntax::Definition*> m_symtab;
	ParsedFileMap m_parsed_files;  // Owns the ParsedFile pointers.
	ModuleMap m_modules;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_ENVIRONMENT_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
