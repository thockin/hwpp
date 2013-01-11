/* Copyright (c) Tim Hockin, 2008 */
#ifndef HWPP_ALIAS_H__
#define HWPP_ALIAS_H__

#include "hwpp.h"
#include "dirent.h"
#include "path.h"

namespace hwpp {

/*
 * Alias - a symbolic link to a dirent.
 */
class Alias: public Dirent
{
    public:
	explicit Alias(const Path &path)
	    : Dirent(DIRENT_TYPE_ALIAS), m_path(path)
	{
	}

	const Path &
	link_path() const
	{
		return m_path;
	}

    private:
	Path m_path;
};
typedef boost::shared_ptr<Alias> AliasPtr;
typedef boost::shared_ptr<const Alias> ConstAliasPtr;

#define new_hwpp_alias(...) ::hwpp::AliasPtr(new ::hwpp::Alias(__VA_ARGS__))

// const
inline ConstAliasPtr
alias_from_dirent(const ConstDirentPtr &dirent)
{
	if (dirent->is_alias()) {
		return static_pointer_cast<const Alias>(dirent);
	}
	throw Dirent::ConversionError("non-alias dirent used as alias");
}
// non-const
inline AliasPtr
alias_from_dirent(const DirentPtr &dirent)
{
	const ConstDirentPtr &const_dirent = dirent;
	return const_pointer_cast<Alias>(
	       alias_from_dirent(const_dirent));
}

}  // namespace hwpp

#endif // HWPP_ALIAS_H__
