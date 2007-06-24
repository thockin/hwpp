/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_SCOPE_H__
#define PP_PP_SCOPE_H__

#include "pp.h"
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

class pp_scope: public pp_container
{
    public:
	/*
	 * pp_scope::dirent
	 */
	class dirent
	{
	    public:
		explicit dirent(pp_register_ptr reg)
		    : m_type(DIRENT_REGISTER), m_register(reg) {}
		explicit dirent(pp_scope_ptr scope)
		    : m_type(DIRENT_SCOPE), m_scope(scope) {}
		explicit dirent(pp_field_ptr field)
		    : m_type(DIRENT_FIELD), m_field(field) {}
		~dirent() {}

		bool
		is_register() const
		{
			return m_type == DIRENT_REGISTER;
		}
		pp_register_ptr
		as_register() const
		{
			if (!is_register()) {
				throw std::runtime_error(
				    "non-register dirent used as register");
			}
			return m_register;
		}

		bool
		is_scope() const
		{
			return m_type == DIRENT_SCOPE;
		}
		pp_scope_ptr
		as_scope() const
		{
			if (!is_scope()) {
				throw std::runtime_error(
				    "non-scope dirent used as scope");
			}
			return m_scope;
		}

		bool
		is_field() const
		{
			return m_type == DIRENT_FIELD;
		}
		//FIXME: return pp_const_field_ptr?
		pp_field_ptr
		as_field() const
		{
			if (!is_field()) {
				throw std::runtime_error(
				    "non-field dirent used as field");
			}
			return m_field;
		}

	    private:
		enum {
			DIRENT_REGISTER,
			DIRENT_SCOPE,
			DIRENT_FIELD,
		} m_type;
		pp_register_ptr m_register;
		pp_scope_ptr m_scope;
		pp_field_ptr m_field;
	};

    public:
	explicit pp_scope() {}
	virtual ~pp_scope() {}

	//FIXME: access methods for the raw vectors to be read-only?

	/*
	 * pp_scope::add_datatype(name, datatype)
	 *
	 * Add a named datatype to this scope.
	 */
	void
	add_datatype(const string &name, const pp_datatype_ptr &datatype)
	{
		datatypes.insert(name, datatype);
	}

	/*
	 * pp_scope::add_register(name, reg)
	 *
	 * Add a named register to this scope.
	 */
	void
	add_register(const string &name, const pp_register_ptr &reg)
	{
		dirents.insert(name, dirent(reg));
	}

	/*
	 * pp_scope::add_field(name, field)
	 *
	 * Add a named field to this scope.
	 */
	void
	add_field(const string &name, const pp_field_ptr &field)
	{
		dirents.insert(name, dirent(field));
	}

	/*
	 * pp_scope::add_scope(name, scope)
	 *
	 * Add a named scope to this scope.
	 */
	void
	add_scope(const string &name, const pp_scope_ptr &scope)
	{
		pp_container_ptr tmp = shared_from_this();
		scope->set_parent(tmp);
		dirents.insert(name, dirent(scope));
	}

	keyed_vector<string, pp_datatype_ptr> datatypes;
	keyed_vector<string, dirent> dirents;
};

#define new_pp_scope(...) pp_scope_ptr(new pp_scope(__VA_ARGS__))

#endif // PP_PP_SCOPE_H__
