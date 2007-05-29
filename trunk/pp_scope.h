0a1,94
> /* Copyright (c) Tim Hockin, 2007 */
> #ifndef PP_PP_SCOPE_H__
> #define PP_PP_SCOPE_H__
> 
> #include "pp.h"
> #include "pp_container.h"
> #include "keyed_vector.h"
> #include "pp_datatype.h"
> #include "pp_register.h"
> #include "pp_field.h"
> #include "pp_dirent.h"
> 
> /*
>  * pp_scope - a lexical scope.
>  *
>  * Constructors:
>  *	()
>  *
>  * Notes:
>  */
> class pp_scope;
> typedef boost::shared_ptr<pp_scope> pp_scope_ptr;
> 
> class pp_scope: public pp_dirent, public pp_container {
>     public:
> 	explicit pp_scope(): pp_dirent(PP_DIRENT_SCOPE) {}
> 	virtual ~pp_scope() {}
> 
> 	//FIXME: access methods for the raw vectors to be read-only?
> 
> 	/*
> 	 * pp_scope::add_constant(name, value)
> 	 *
> 	 * Add a named constant to this scope.
> 	 */
> 	keyed_vector<string, pp_value> constants;
> 	void add_constant(const string &name, const pp_value value) {
> 		constants.insert(name, value);
> 	}
> 
> 	/*
> 	 * pp_scope::add_datatype(name, datatype)
> 	 *
> 	 * Add a named datatype to this scope.
> 	 */
> 	keyed_vector<string, pp_datatype_ptr> datatypes;
> 	void add_datatype(const string &name,
> 	    const pp_datatype_ptr &datatype) {
> 		datatypes.insert(name, datatype);
> 	}
> 
> 	/*
> 	 * pp_scope::add_register(name, reg)
> 	 *
> 	 * Add a named register to this scope.
> 	 */
> 	keyed_vector<string, pp_register_ptr> registers;
> 	void add_register(const string &name, const pp_register_ptr &reg) {
> 		registers.insert(name, reg);
> 	}
> 
> 	//FIXME: generate a random name (.anon123) for anonymous scopes.  Or
> 	//do we support anon scopes at all?  How do you address them?
> 	keyed_vector<string, pp_dirent_ptr> dirents;
> 
> 	/*
> 	 * pp_scope::add_field(name, field)
> 	 *
> 	 * Add a named field to this scope.
> 	 */
> 	void add_field(const string &name, const pp_field_ptr &field) {
> 		dirents.insert(name, field);
> 	}
> 
> 	/*
> 	 * pp_scope::add_scope(name, scope)
> 	 *
> 	 * Add a named scope to this scope.
> 	 */
> 	void add_scope(const string &name, const pp_scope_ptr &scope) {
> 		pp_container_ptr tmp = shared_from_this();
> 		scope->set_parent(tmp);
> 		dirents.insert(name, scope);
> 	}
> };
> 
> inline pp_scope_ptr
> pp_scope_from_dirent(pp_dirent_ptr dirent)
> {
> 	//FIXME: check type?  return error?
> 	return boost::static_pointer_cast<pp_scope>(dirent);
> }
> 
> #endif // PP_PP_SCOPE_H__
