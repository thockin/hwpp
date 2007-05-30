/* Copyright (c) Tim Hockin, 2007 */
#ifndef PP_PP_SCOPE_H__
#define PP_PP_SCOPE_H__

#include "pp.h"
#include "pp_container.h"
#include "keyed_vector.h"
#include "pp_datatype.h"
#include "pp_register.h"
#include "pp_field.h"

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

class pp_scope: public pp_container {
    public:
	typedef enum {
		DIRENT_SCOPE,
		DIRENT_FIELD,
	} dirent_type;

    private:
	class dirent {
	    public:
		explicit dirent(pp_scope_ptr scope)
		    : m_type(DIRENT_SCOPE), m_scope(scope) {}
		explicit dirent(pp_field_ptr field)
		    : m_type(DIRENT_FIELD), m_field(field) {}
		~dirent() {}

		dirent_type type() const {
			return m_type;
		}

		bool is_scope() const {
			return m_type == DIRENT_SCOPE;
		}
		pp_scope_ptr scope() const {
			return m_scope;
		}

		bool is_field() const {
			return m_type == DIRENT_FIELD;
		}
		pp_field_ptr field() const {
			return m_field;
		}

	    private:
		dirent_type m_type;
		pp_scope_ptr m_scope;
		pp_field_ptr m_field;
	};

    public:
	explicit pp_scope() {}
	virtual ~pp_scope() {}

	//FIXME: access methods for the raw vectors to be read-only?

	/*
	 * pp_scope::add_constant(name, value)
	 *
	 * Add a named constant to this scope.
	 */
	void add_constant(const string &name, const pp_value value) {
		constants.insert(name, value);
	}

	/*
	 * pp_scope::add_datatype(name, datatype)
	 *
	 * Add a named datatype to this scope.
	 */
	void add_datatype(const string &name,
	    const pp_datatype_ptr &datatype) {
		datatypes.insert(name, datatype);
	}

	/*
	 * pp_scope::add_register(name, reg)
	 *
	 * Add a named register to this scope.
	 */
	void add_register(const string &name, const pp_register_ptr &reg) {
		registers.insert(name, reg);
	}

	/*
	 * pp_scope::add_field(name, field)
	 *
	 * Add a named field to this scope.
	 */
	void add_field(const string &name, const pp_field_ptr &field) {
		dirents.insert(name, dirent(field));
	}

	//FIXME: generate a random name (.anon123) for anonymous scopes.  Or
	//do we support anon scopes at all?  How do you address them?
	/*
	 * pp_scope::add_scope(name, scope)
	 *
	 * Add a named scope to this scope.
	 */
	void add_scope(const string &name, const pp_scope_ptr &scope) {
		pp_container_ptr tmp = shared_from_this();
		scope->set_parent(tmp);
		dirents.insert(name, dirent(scope));
	}

	keyed_vector<string, pp_value> constants;
	keyed_vector<string, pp_datatype_ptr> datatypes;
	keyed_vector<string, pp_register_ptr> registers;
	keyed_vector<string, dirent> dirents;

};

#endif // PP_PP_SCOPE_H__
