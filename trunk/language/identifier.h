#ifndef PP_LANGUAGE_IDENTIFIER_H__
#define PP_LANGUAGE_IDENTIFIER_H__

#include "pp/pp.h"
#include <string>

namespace pp {
namespace language {

class Identifier {
    public:
	Identifier(const string &module, const string &symbol)
	    : m_module(module), m_symbol(symbol)
	{
	}
	Identifier(const string &symbol)
	    : m_module(), m_symbol(symbol)
	{
	}

	string module() const
	{
		return m_module;
	}

	string symbol() const
	{
		return m_symbol;
	}

    private:
	string m_module;
	string m_symbol;
};

}  // namespace language
}  // namespace pp

#endif  // PP_LANGUAGE_IDENTIFIER_H__
