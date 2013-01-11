/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 */

#include "hwpp.h"
#include "util/printfxx.h"
#include "fake_language.h"
#include "language.h"
#include "runtime.h"
#include "field.h"
#include "field_types.h"
#include "path.h"
#include "dirent.h"
#include "register.h"
#include "register_types.h"
#include "scope.h"
#include "datatype_types.h"
#include "alias.h"

#include <stdexcept>

namespace hwpp {

//
// Resolve a path to a dirent, relative to the current scope.
//
static ConstDirentPtr
fkl_get_dirent(const ParseLocation &loc, const Path &path)
{
	try {
		const ConstDirentPtr &de =
		    global_runtime()->current_context()->lookup_dirent(path,
		        Scope::RESOLVE_ALIAS);
		if (de == NULL) {
			throw Path::NotFoundError(path.to_string());
		}
		return de;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
ConstDirentPtr
fkl_get_dirent(const ParseLocation &loc, const string &path_str)
{
	try {
		return fkl_get_dirent(loc, Path(path_str));;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}

//
// Resolve a path to a field, relative to the current scope.
//
ConstFieldPtr
fkl_get_field(const ParseLocation &loc, const string &path_str)
{
	const ConstDirentPtr &de = fkl_get_dirent(loc, path_str);
	if (de->is_field()) {
		return field_from_dirent(de);
	}
	throw Dirent::ConversionError("path is not a field: " + path_str);
}

//
// Resolve a path to a register, relative to the current scope.
//
ConstRegisterPtr
fkl_get_register(const ParseLocation &loc, const string &path_str)
{
	// magic registers
	extern ConstRegisterPtr magic_zeros;
	extern ConstRegisterPtr magic_ones;

	if (path_str == "%0")
		return magic_zeros;
	if (path_str == "%1")
		return magic_ones;

	const ConstDirentPtr &de = fkl_get_dirent(loc, path_str);
	if (de->is_register()) {
		return register_from_dirent(de);
	}
	throw Dirent::ConversionError("path is not a register: "
			+ path_str);
}

//
// Check whether a path resolves successfully.
//
bool
fkl_defined(const ParseLocation &loc, const string &path_str)
{
	try {
		Path path(path_str);
		return global_runtime()->current_context()->dirent_defined(path);
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
static bool
fkl_defined(const ParseLocation &loc, const Path &path)
{
	(void)loc;
	return global_runtime()->current_context()->dirent_defined(path);
}

//
// Read and write a field or register.
//
static Value
fkl_read(const ParseLocation &loc, const Path &path)
{
	try {
		const ConstDirentPtr &de = fkl_get_dirent(loc, path);
		if (de->is_register()) {
			return register_from_dirent(de)->read();
		} else if (de->is_field()) {
			return field_from_dirent(de)->read();
		} else if (de->is_alias()) {
			return fkl_read(loc,
			                alias_from_dirent(de)->link_path());
		}
		throw Dirent::ConversionError(
		    sprintfxx("path is not a register or field: %s (%s)",
		              path, de->dirent_type()));
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
Value
fkl_read(const ParseLocation &loc, const string &path_str)
{
	try {
		return fkl_read(loc, Path(path_str));
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
static void
fkl_write(const ParseLocation &loc, const Path &path, const Value &value)
{
	try {
		const ConstDirentPtr &de = fkl_get_dirent(loc, path);
		if (de->is_register()) {
			register_from_dirent(de)->write(value);
		} else if (de->is_field()) {
			field_from_dirent(de)->write(value);
		} else if (de->is_alias()) {
			fkl_write(loc, alias_from_dirent(de)->link_path(),
			          value);
		} else {
			throw Dirent::ConversionError(
			    sprintfxx(
			        "path is not a register or field: %s (%s)",
			        path, de->dirent_type()));
		}
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
void
fkl_write(const ParseLocation &loc,
          const string &path_str, const Value &value)
{
	try {
		fkl_write(loc, Path(path_str), value);
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}

//
// Read and wite regbits.
//
Value
fkl_read(const ParseLocation &loc, const RegBits &bits)
{
	(void)loc;
	return bits.read();
}
void
fkl_write(const ParseLocation &loc,
          const RegBits &bits, const Value &value)
{
	(void)loc;
	bits.write(value);
}

// Validate a scope name.
static void
fkl_validate_scope_name(const Path::Element &elem, const ParseLocation &loc)
{
	if (elem.name()[0] == '%') {
		throw ParseError("invalid scope name: " + elem.name(), loc);
	}
}

//
// Start a new scope.
//
void
fkl_open_scope(const ParseLocation &loc,
               const string &name, const BindingPtr &binding)
{
	DTRACE(TRACE_SCOPES, "open scope: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	try {
		Path::Element elem(name);

		// make sure the name is valid
		fkl_validate_scope_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, Path(elem))) {
			WARN(sprintfxx("%s: '%s' redefined", loc, name));
		}

		// make a new scope and link it into the tree
		ScopePtr scope_ptr = new_hwpp_scope(binding);
		scope_ptr->set_parent(global_runtime()->current_context()->scope());

		// add the new scope to the parent
		global_runtime()->current_context()->add_dirent(elem, scope_ptr);

		// save the current scope, and set the new scope
		global_runtime()->context_push(new_hwpp_context(elem, scope_ptr));
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}

//
// Close the current scope.
//
void
fkl_close_scope(const ParseLocation &loc)
{
	(void)loc;
	DTRACE(TRACE_SCOPES, "close scope: "
	                     + global_runtime()->current_context()->name());

	// retore the previous context
	global_runtime()->context_pop();
}

//
// Define a bookmark
//
void
fkl_bookmark(const ParseLocation &loc, const string &name)
{
	if (!lang_valid_bookmark_name(name)) {
		throw ParseError("invalid bookmark name: " + name, loc);
	}
	global_runtime()->current_context()->add_bookmark(name);
}

// Validate a register name.
static void
fkl_validate_register_name(const Path::Element &elem,
                           const ParseLocation &loc)
{
	if (elem.name()[0] != '%') {
		throw ParseError("invalid register name: " + elem.name(),
		                     loc);
	}
}

//
// Define a register.
//
RegisterPtr
fkl_reg(const ParseLocation &loc,
        const string &name, const Value &address, BitWidth width)
{
	// check that we have a current binding
	ConstBindingPtr cur_binding
	    = global_runtime()->current_context()->binding();
	DASSERT_MSG(cur_binding, "no binding for register " + name);
	return fkl_reg(loc, name, cur_binding, address, width);
}
RegisterPtr
fkl_reg(const ParseLocation &loc,
        const string &name, const ConstBindingPtr &binding,
        const Value &address, BitWidth width)
{
	DTRACE(TRACE_REGS, "reg: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	try {
		Path::Element elem(name);

		// make sure the name is valid
		fkl_validate_register_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, Path(elem))) {
			WARN(sprintfxx("%s: '%s' redefined", loc, name));
		}

		RegisterPtr reg_ptr = new_hwpp_bound_register(
				binding, address, width);
		global_runtime()->current_context()->add_dirent(elem, reg_ptr);
		return reg_ptr;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
RegisterPtr
fkl_reg(const ParseLocation &loc,
        const Value &address, BitWidth width)
{
	// check that we have a current binding
	ConstBindingPtr cur_binding
	    = global_runtime()->current_context()->binding();
	DASSERT_MSG(cur_binding, "no binding for register");
	return fkl_reg(loc, cur_binding, address, width);
}
RegisterPtr
fkl_reg(const ParseLocation &loc,
        const ConstBindingPtr &binding, const Value &address,
        BitWidth width)
{
	(void)loc;
	return new_hwpp_bound_register(binding, address, width);
}


//
// Define a proc-register
//
RegisterPtr
fkl_reg(const ParseLocation &loc,
        const string &name, const RwProcsPtr &access, BitWidth width)
{
	DTRACE(TRACE_REGS, "reg: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	try {
		Path::Element elem(name);

		// make sure the name is valid
		fkl_validate_register_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, Path(elem))) {
			WARN(sprintfxx("%s: '%s' redefined", loc, name));
		}

		RegisterPtr reg_ptr
		    = new_hwpp_proc_register(access, width, global_runtime());
		global_runtime()->current_context()->add_dirent(elem, reg_ptr);
		return reg_ptr;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
RegisterPtr
fkl_reg(const ParseLocation &loc, const RwProcsPtr &access, BitWidth width)
{
	(void)loc;
	return new_hwpp_proc_register(access, width, global_runtime());
}

//
// Define a regbits from a register name and bit range.
//
RegBits
fkl_bits(const ParseLocation &loc, const string &regname)
{
	const ConstRegisterPtr &reg = fkl_get_register(loc, regname);
	return fkl_bits(loc, reg);
}
RegBits
fkl_bits(const ParseLocation &loc, const string &regname, BitWidth bit)
{
	const ConstRegisterPtr &reg = fkl_get_register(loc, regname);
	return fkl_bits(loc, reg, bit);
}
RegBits
fkl_bits(const ParseLocation &loc,
         const string &regname, BitWidth hi_bit, BitWidth lo_bit)
{
	const ConstRegisterPtr &reg = fkl_get_register(loc, regname);
	return fkl_bits(loc, reg, hi_bit, lo_bit);
}
RegBits
fkl_bits(const ParseLocation &loc, const ConstRegisterPtr &reg)
{
	(void)loc;
	return RegBits(reg, reg->width()-1, 0);
}
RegBits
fkl_bits(const ParseLocation &loc,
         const ConstRegisterPtr &reg, BitWidth bit)
{
	(void)loc;
	return RegBits(reg, bit, bit);
}
RegBits
fkl_bits(const ParseLocation &loc,
         const ConstRegisterPtr &reg, BitWidth hi_bit, BitWidth lo_bit)
{
	(void)loc;
	return RegBits(reg, hi_bit, lo_bit);
}


// Validate a field name.
static void
fkl_validate_field_name(const Path::Element &elem, const ParseLocation &loc)
{
	if (elem.name()[0] == '%') {
		throw ParseError("invalid field name: " + elem.name(), loc);
	}
}

//
// Define a field as a set of register-bits.
//
FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const RegBits &bits)
{
	DTRACE(TRACE_FIELDS, "direct field: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);

	try {
		Path::Element elem(name);

		// make sure the name is valid
		fkl_validate_field_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, Path(elem))) {
			WARN(sprintfxx("%s: '%s' redefined", loc, name));
		}

		// create a field and add it to the current scope
		DirectFieldPtr field_ptr = new_hwpp_direct_field(type, bits);
		global_runtime()->current_context()->add_dirent(elem, field_ptr);
		return field_ptr;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type, const RegBits &bits)
{
	return fkl_field(loc, name,
	                 global_runtime()->current_context()->resolve_datatype(type),
	                 bits);
}


//
// Define a field as a constant value.
//
FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const Value &value)
{
	DTRACE(TRACE_FIELDS, "constant field: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);

	try {
		Path::Element elem(name);

		// make sure the name is valid
		fkl_validate_field_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, Path(elem))) {
			WARN(sprintfxx("%s: '%s' redefined", loc, name));
		}

		// create a field and add it to the current scope
		ConstantFieldPtr field_ptr = new_hwpp_constant_field(type, value);
		global_runtime()->current_context()->add_dirent(elem, field_ptr);
		return field_ptr;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type, const Value &value)
{
	return fkl_field(loc, name,
	                 global_runtime()->current_context()->resolve_datatype(type),
	                 value);
}
//
// Define a field as a set of procedures.
//
FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const ConstDatatypePtr &type,
          const RwProcsPtr &access)
{
	DTRACE(TRACE_FIELDS, "proc field: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);
	DASSERT_MSG(access, "found NULL accessors for field " + name);

	try {
		Path::Element elem(name);

		// make sure the name is valid
		fkl_validate_field_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, Path(elem))) {
			WARN(sprintfxx("%s: '%s' redefined", loc, name));
		}

		// create a field and add it to the current scope
		ProcFieldPtr field_ptr = new_hwpp_proc_field(type, access);
		global_runtime()->current_context()->add_dirent(elem, field_ptr);
		return field_ptr;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}
FieldPtr
fkl_field(const ParseLocation &loc,
          const string &name, const string &type,
          const RwProcsPtr &access)
{
	return fkl_field(loc, name,
	                 global_runtime()->current_context()->resolve_datatype(type),
	                 access);
}

//
// Define an alias
//
AliasPtr
fkl_alias(const ParseLocation &loc, const string &name, const string &target)
{
	DTRACE(TRACE_REGS, "alias: " + name + " -> " + target);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	try {
		Path::Element elem(name);

		// note: this is not a debug-only test
		if (fkl_defined(loc, Path(elem))) {
			WARN(sprintfxx("%s: '%s' redefined", loc, name));
		}

		Path path = global_runtime()->current_context()->resolve_path(target);
		if (!path.is_initialized()) {
			throw Path::NotFoundError(target);
		}
		AliasPtr alias_ptr = new_hwpp_alias(path);
		global_runtime()->current_context()->add_dirent(elem, alias_ptr);
		return alias_ptr;
	} catch (Path::InvalidError &e) {
		throw ParseError(e.what(), loc);
	}
}

// Validate a datatype name.
void
fkl_validate_type_name(const string &name, const ParseLocation &loc)
{
	if (!lang_valid_datatype_name(name)) {
		throw ParseError("invalid datatype name: " + name, loc);
	}
}

//
// Define a IntDatatype.
//
DatatypePtr
fkl_int(const ParseLocation &loc, const string &name, const string &units)
{
	DTRACE(TRACE_TYPES, "int: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	IntDatatypePtr int_ptr = new_hwpp_int_datatype(units);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, int_ptr);
	}
	return int_ptr;
}

//
// Define a HexDatatype.
//
DatatypePtr
fkl_hex(const ParseLocation &loc,
        const string &name, const BitWidth width, const string &units)
{
	DTRACE(TRACE_TYPES, "hex: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	HexDatatypePtr hex_ptr = new_hwpp_hex_datatype(width, units);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, hex_ptr);
	}
	return hex_ptr;
}

//
// Define a BitmaskDatatype.
//
DatatypePtr
fkl_bitmask(const ParseLocation &loc,
            const string &name, const FklKvPairList &kvlist)
{
	DTRACE(TRACE_TYPES, "bitmask: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	BitmaskDatatypePtr bitmask_ptr = new_hwpp_bitmask_datatype(kvlist);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, bitmask_ptr);
	}
	return bitmask_ptr;
}

//
// Define a StringDatatype.
//
DatatypePtr
fkl_string(const ParseLocation &loc, const string &name)
{
	DTRACE(TRACE_TYPES, "string: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	StringDatatypePtr string_ptr = new_hwpp_string_datatype();
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, string_ptr);
	}
	return string_ptr;
}

//
// Define a EnumDatatype.
//
DatatypePtr
fkl_enum(const ParseLocation &loc,
         const string &name, const FklKvPairList &kvlist)
{
	DTRACE(TRACE_TYPES, "enum: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	EnumDatatypePtr enum_ptr = new_hwpp_enum_datatype(kvlist);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, enum_ptr);
	}
	return enum_ptr;
}

//
// Define a MultiDatatype.
//
DatatypePtr
fkl_multi(const ParseLocation &loc,
	      const string &name, const FklRangeList &rangelist)
{
	DTRACE(TRACE_TYPES, "multi: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	MultiDatatypePtr multi_ptr = new_hwpp_multi_datatype();
	for (size_t i = 0; i < rangelist.size(); i++) {
		multi_ptr->add_range(rangelist[i].type,
				rangelist[i].low, rangelist[i].high);
	}
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, multi_ptr);
	}
	return multi_ptr;
}

//
// Define a BoolDatatype.
//
DatatypePtr
fkl_bool(const ParseLocation &loc,
         const string &name, const string &true_str, const string &false_str)
{
	DTRACE(TRACE_TYPES, "bool: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	BoolDatatypePtr bool_ptr = new_hwpp_bool_datatype(
			true_str, false_str);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, bool_ptr);
	}
	return bool_ptr;
}

//
// Define a FixedDatatype.
//
DatatypePtr
fkl_fixed(const ParseLocation &loc,
          const string &name, int nbits, const string &units)
{
	DTRACE(TRACE_TYPES, "fixed: " + name);

	DASSERT_MSG(!global_runtime()->current_context()->is_readonly(),
		"current_context is read-only");

	FixedDatatypePtr fixed_ptr = new_hwpp_fixed_datatype(nbits, units);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		global_runtime()->current_context()->add_datatype(name, fixed_ptr);
	}
	return fixed_ptr;
}

}  // namespace hwpp
