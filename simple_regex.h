/* Copyright (c) Tim Hockin, 2008 */
#ifndef PP_REGEX_H__
#define PP_REGEX_H__

#include <stdexcept>
#include <sys/types.h>
#include <regex.h>

// This is a simple regex class.  Nothing fancy, and does not require a
// library, as does boost.regex.
// Example:
// 	regex re("^foo.*bar");
// 	if (re.matches("foobar"))
// 		std::cout << "yes!" << std::endl;
class regex
{
    private:
	string m_re_string;
	unsigned m_flags;
	regex_t m_c_regex;

    public:
	// global defaults
	static const unsigned default_flags = (REG_EXTENDED | REG_NOSUB);
	static const unsigned default_exec_flags = 0;

	// ctor from string
	regex(const string &re_str, unsigned flags = default_flags)
	    : m_re_string(re_str), m_flags(flags)
	{
		compile();
	}
	// copy ctor
	regex(const regex &that)
	{
		*this = that;
	}
	// assignment operator
	regex &
	operator=(const regex &that)
	{
		if (this != &that) {
			m_re_string = that.m_re_string;
			m_flags = that.m_flags;
			compile();
		}
		return *this;
	}
	// dtor
	~regex()
	{
		regfree(&m_c_regex);
	}

	// see if a string matches against this regex
	bool
	matches(const string &test_string,
			unsigned exec_flags = default_exec_flags) const
	{
		int ret;
		ret = regexec(&m_c_regex, test_string.c_str(),
				0, NULL, exec_flags);
		return (ret == 0);
	}

	// setters
	void
	set_ignore_case(bool val)
	{
		unsigned flag = REG_ICASE;
		if (val) {
			set_flag(flag);
		} else {
			clear_flag(flag);
		}
	}
	void
	set_match_newline(bool val)
	{
		unsigned flag = REG_NEWLINE;
		if (val) {
			set_flag(flag);
		} else {
			clear_flag(flag);
		}
	}
	void
	set_extended(bool val)
	{
		unsigned flag = REG_EXTENDED;
		if (val) {
			set_flag(flag);
		} else {
			clear_flag(flag);
		}
	}

	// getters
	const string &
	re_string() const
	{
		return m_re_string;
	}
	const unsigned
	flags() const
	{
		return m_flags;
	}
	bool
	is_ignore_case() const
	{
		return (m_flags & REG_ICASE);
	}
	bool
	is_match_newline() const
	{
		return (m_flags & REG_NEWLINE);
	}
	bool
	is_extended() const
	{
		return (m_flags & REG_EXTENDED);
	}
	const regex_t *
	c_regex() const
	{
		return &m_c_regex;
	}

    private:
	void
	compile()
	{
		int ret;
		ret = regcomp(&m_c_regex, m_re_string.c_str(), m_flags);
		if (ret != 0) {
			char err_str[1024];
			regerror(ret, &m_c_regex, err_str, sizeof(err_str));
			throw std::invalid_argument(err_str);
		}
	}

	void
	set_flag(unsigned flag)
	{
		if (!(m_flags & flag)) {
			m_flags |= flag;
			compile();
		}
	}

	void
	clear_flag(unsigned flag)
	{
		if (m_flags & flag) {
			m_flags &= ~flag;
			compile();
		}
	}
};

#endif // PP_REGEX_H__
