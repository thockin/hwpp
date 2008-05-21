/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_PP_LANGUAGE_H__
#define PP_PP_LANGUAGE_H__

#include "pp.h"
#include <exception>

class parse_location
{
    private:
	const char *m_file;
	int m_line;
	const char *m_function;

    public:
	parse_location()
	    : m_file(NULL), m_line(-1), m_function(NULL)
	{
	}
	parse_location(const char *file, int line, const char *func)
	    : m_file(file), m_line(line), m_function(func)
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
	const char *
	function() const
	{
		return m_function;
	}
};
#define THIS_LOCATION	parse_location(__FILE__, __LINE__, __func__)

class pp_parse_error: public std::exception
{
    private:
	string m_error;
	parse_location m_location;
	bool m_location_valid;

    public:
	explicit pp_parse_error(const string &error)
	    : m_error(error), m_location_valid(false)
	{
	}
	~pp_parse_error() throw()
	{
	}

	void
	set_location(const parse_location &loc)
	{
		m_location = loc;
		m_location_valid = true;
	}

	virtual const char *
	what() const throw()
	{
		string full;
		if (m_location_valid) {
			full += to_string(m_location.file())
			     + ":"
			     + to_string(m_location.line())
			     + ": "
			     + to_string(m_location.function())
			     + ": ";
		}
		full += m_error;
		return full.c_str();
	}
};

extern bool
lang_valid_datatype_name(const string &name);

extern bool
lang_valid_datatype_key(const string &name);

#endif // PP_PP_LANGUAGE_H__
