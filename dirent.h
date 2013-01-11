/* Copyright (c) Tim Hockin, 2007 */
#ifndef HWPP_DIRENT_H__
#define HWPP_DIRENT_H__

#include "hwpp.h"
#include <ostream>

namespace hwpp {

// The specific type of a dirent.
typedef enum {
	DIRENT_TYPE_REGISTER,
	DIRENT_TYPE_FIELD,
	DIRENT_TYPE_SCOPE,
	DIRENT_TYPE_ARRAY,
	DIRENT_TYPE_ALIAS,
	DIRENT_TYPE_MAX,
} DirentType;

inline std::ostream &
operator<<(std::ostream& o, const DirentType &type)
{
	if (type >= DIRENT_TYPE_MAX) {
		throw std::out_of_range(
		    "invalid DirentType: " + to_string((int)type));
	}

	// Note: this is indexed by DirentType above.
	static const char *dirent_type_strings[] = {
		"Register",
		"Field",
		"Scope",
		"Array",
		"Alias",
	};

	return o << dirent_type_strings[type];
}

/*
 * Dirent - a directory entry in a scope
 */
class Dirent
{
    public:
	// a dirent conversion error
	struct ConversionError: public std::runtime_error
	{
		explicit ConversionError(const string &str)
		    : runtime_error(str)
		{
		}
	};

	explicit Dirent(DirentType type): m_type(type)
	{
		DASSERT_MSG(type < DIRENT_TYPE_MAX, "invalid DirentType");
		DTRACE(TRACE_DIRENTS && TRACE_LIFETIMES,
		       sprintfxx("new dirent @ %p", this));
	}
	virtual ~Dirent()
	{
		DTRACE(TRACE_DIRENTS && TRACE_LIFETIMES,
		       sprintfxx("del dirent @ %p", this));
	}

	/*
	 * Dirent::dirent_type()
	 *
	 * Get the DirentType of this object.
	 */
	DirentType
	dirent_type() const
	{
		return m_type;
	}

	/*
	 * Dirent::is_*()
	 *
	 * Check the type of this object.
	 */
	bool
	is_register() const
	{
		return (m_type == DIRENT_TYPE_REGISTER);
	}
	bool
	is_field() const
	{
		return (m_type == DIRENT_TYPE_FIELD);
	}
	bool
	is_scope() const
	{
		return (m_type == DIRENT_TYPE_SCOPE);
	}
	bool
	is_array() const
	{
		return (m_type == DIRENT_TYPE_ARRAY);
	}
	bool
	is_alias() const
	{
		return (m_type == DIRENT_TYPE_ALIAS);
	}

    private:
	DirentType m_type;
};
typedef boost::shared_ptr<Dirent> DirentPtr;
typedef boost::shared_ptr<const Dirent> ConstDirentPtr;

}  // namespace hwpp

#endif // HWPP_DIRENT_H__
