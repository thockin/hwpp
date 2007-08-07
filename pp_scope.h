/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_SCOPE_H__
#define PP_PP_SCOPE_H__

#include "pp.h"
#include "pp_dirent.h"
#include "pp_container.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_field.h"

#include <stdexcept>

/*
 * pp_scope - a lexical scope.
 *
 * Constructors:
 *	()
 *
 * Notes:
 */
class pp_scope;
typedef boost::shared_ptr<pp_scope> pp_scope_ptr;
typedef boost::shared_ptr<const pp_scope> pp_const_scope_ptr;

class pp_scope: public pp_dirent, public pp_container
{
    protected:
	explicit pp_scope(pp_dirent_type detype): pp_dirent(detype) {}
    public:
	explicit pp_scope(): pp_dirent(PP_DIRENT_SCOPE) {}
	virtual ~pp_scope() {}

	/*
	 * pp_scope::add_register(name, reg)
	 *
	 * Add a named register to this scope.
	 */
	void
	add_register(const string &name, pp_register_ptr reg)
	{
		m_dirents.insert(name, reg);
	}

	/*
	 * pp_scope::add_field(name, field)
	 *
	 * Add a named field to this scope.
	 */
	void
	add_field(const string &name, pp_field_ptr field)
	{
		m_dirents.insert(name, field);
	}

	/*
	 * pp_scope::add_scope(name, scope)
	 *
	 * Add a named scope to this scope.
	 */
	void
	add_scope(const string &name, pp_scope_ptr scope)
	{
		scope->set_parent(this);
		m_dirents.insert(name, scope);
	}
};

inline const pp_scope *
pp_scope_from_dirent(pp_const_dirent_ptr dirent)
{
	if (dirent->dirent_type() != PP_DIRENT_SCOPE) {
		throw std::runtime_error("non-scope dirent used as scope");
	}
	return static_cast<const pp_scope *>(dirent.get());
}

#define new_pp_scope(...) pp_scope_ptr(new pp_scope(__VA_ARGS__))

#endif // PP_PP_SCOPE_H__
