// Language-related errors.  These are the only errors that should propagate
// up out of the language code.

#ifndef PP_LANGUAGE_ERRORS_H__
#define PP_LANGUAGE_ERRORS_H__

// The grammar.l and grammar.y files include this header, which will screw
// them up if they include their own auto-generated headers, so don't include
// auto.*.h here.
#include "pp.h"
#include <stdexcept>
#include <string>

namespace pp {
namespace language {

struct LanguageError: public std::runtime_error
{
	LanguageError(const Parser::Position &pos, const std::string &str)
	    : runtime_error(sprintfxx("[%s:%d] : %s", pos.file, pos.line, str))
	{
	}
};

struct SyntaxError: public LanguageError
{
	SyntaxError(const Parser::Position &pos, const std::string &str)
	    : LanguageError(pos, str)
	{
	}
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_ERRORS_H__

// vim: set ai tabstop=4 shiftwidth=4 noexpandtab:
