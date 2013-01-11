/* Copyright (c) Tim Hockin, 2008 */
#ifndef HWPP_LANGUAGE_H__
#define HWPP_LANGUAGE_H__

#include "hwpp.h"
#include <exception>
#include <ostream>

namespace hwpp {

class ParseLocation
{
    private:
	const char *m_file;
	int m_line;

    public:
	ParseLocation()
	    : m_file(NULL), m_line(-1)
	{
	}
	ParseLocation(const char *file, int line)
	    : m_file(file), m_line(line)
	{
	}
	const char *
	file() const
	{
		return m_file;
	}
	int
	line() const
	{
		return m_line;
	}
};
// Stream out a parse_location
inline std::ostream &
operator<<(std::ostream& o, const ParseLocation &loc)
{
	return o << loc.file() << ":" << loc.line();
}
#define THIS_LOCATION  ::hwpp::ParseLocation(__FILE__, __LINE__)

class ParseError: public std::exception
{
    private:
	string m_error;
	ParseLocation m_location;
	bool m_location_valid;
	mutable string m_what;

    public:
	// NOTE: taking a string here is a calculated risk.  It could
	// throw during construction, assignment, or what().
	explicit ParseError(const string &error)
	    : m_error(error), m_location_valid(false)
	{
	}
	explicit ParseError(const string &error, const ParseLocation &loc)
	    : m_error(error), m_location(loc), m_location_valid(true)
	{
	}
	~ParseError() throw()
	{
	}

	void
	set_location(const ParseLocation &loc)
	{
		m_location = loc;
		m_location_valid = true;
	}

	virtual const char *
	what() const throw()
	{
		// Yes, using a string could blow up, but I really don't
		// want to deal with resource management or fixed buffers.
		m_what = "";
		if (m_location_valid) {
			m_what += to_string(m_location) + ": ";
		}
		m_what += m_error;
		return m_what.c_str();
	}
};

//
// language token validators
//

extern bool
lang_valid_datatype_name(const string &name);

extern bool
lang_valid_bookmark_name(const string &name);

}  // namespace hwpp

#endif // HWPP_LANGUAGE_H__
