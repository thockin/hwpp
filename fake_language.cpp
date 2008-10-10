/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 */

#include "pp.h"
#include "fake_language.h"
#include "language.h"
#include "runtime.h"
#include "pp_field.h"
#include "pp_fields.h"
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_register.h"
#include "pp_registers.h"
#include "pp_scope.h"
#include "pp_datatypes.h"
#include "pp_alias.h"

#include <stdexcept>

//
// Resolve a path to a dirent, relative to the current scope.
//
static pp_dirent_const_ptr
fkl_get_dirent(const parse_location &loc, const pp_path &path)
{
	try {
		const pp_dirent_const_ptr &de =
		    current_context.lookup_dirent(path,
		        pp_scope::RESOLVE_ALIAS);
		if (de == NULL) {
			throw pp_path::not_found_error(path.to_string());
		}
		return de;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
pp_dirent_const_ptr
fkl_get_dirent(const parse_location &loc, const string &path_str)
{
	try {
		return fkl_get_dirent(loc, pp_path(path_str));;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}

//
// Resolve a path to a field, relative to the current scope.
//
pp_field_const_ptr
fkl_get_field(const parse_location &loc, const string &path_str)
{
	const pp_dirent_const_ptr &de = fkl_get_dirent(loc, path_str);
	if (de->is_field()) {
		return pp_field_from_dirent(de);
	}
	throw pp_dirent::conversion_error("path is not a field: " + path_str);
}

//
// Resolve a path to a register, relative to the current scope.
//
pp_register_const_ptr
fkl_get_register(const parse_location &loc, const string &path_str)
{
	// magic registers
	extern pp_register_const_ptr magic_zeros;
	extern pp_register_const_ptr magic_ones;

	if (path_str == "%0")
		return magic_zeros;
	if (path_str == "%1")
		return magic_ones;

	const pp_dirent_const_ptr &de = fkl_get_dirent(loc, path_str);
	if (de->is_register()) {
		return pp_register_from_dirent(de);
	}
	throw pp_dirent::conversion_error("path is not a register: "
			+ path_str);
}

//
// Check whether a path resolves successfully.
//
bool
fkl_defined(const parse_location &loc, const string &path_str)
{
	try {
		pp_path path(path_str);
		return current_context.dirent_defined(path);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
static bool
fkl_defined(const parse_location &loc, const pp_path &path)
{
	(void)loc;
	return current_context.dirent_defined(path);
}

//
// Read and write a field or register.
//
static pp_value
fkl_read(const parse_location &loc, const pp_path &path)
{
	try {
		const pp_dirent_const_ptr &de = fkl_get_dirent(loc, path);
		if (de->is_register()) {
			return pp_register_from_dirent(de)->read();
		} else if (de->is_field()) {
			return pp_field_from_dirent(de)->read();
		} else if (de->is_alias()) {
			return fkl_read(loc,
			                pp_alias_from_dirent(de)->link_path());
		}
		throw pp_dirent::conversion_error(
		    "path is not a register or field: "
		    + path.to_string()
		    + " (" + to_string(de->dirent_type()) + ")");
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
pp_value
fkl_read(const parse_location &loc, const string &path_str)
{
	try {
		return fkl_read(loc, pp_path(path_str));
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
static void
fkl_write(const parse_location &loc, const pp_path &path, const pp_value &value)
{
	try {
		const pp_dirent_const_ptr &de = fkl_get_dirent(loc, path);
		if (de->is_register()) {
			pp_register_from_dirent(de)->write(value);
		} else if (de->is_field()) {
			pp_field_from_dirent(de)->write(value);
		} else if (de->is_alias()) {
			fkl_write(loc, pp_alias_from_dirent(de)->link_path(),
			          value);
		} else {
			throw pp_dirent::conversion_error(
			    "path is not a register or field: "
			    + path.to_string()
			    + " (" + to_string(de->dirent_type()) + ")");
		}
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
void
fkl_write(const parse_location &loc,
          const string &path_str, const pp_value &value)
{
	try {
		fkl_write(loc, pp_path(path_str), value);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}

//
// Read and wite regbits.
//
pp_value
fkl_read(const parse_location &loc, const pp_regbits &bits)
{
	(void)loc;
	return bits.read();
}
void
fkl_write(const parse_location &loc,
          const pp_regbits &bits, const pp_value &value)
{
	(void)loc;
	bits.write(value);
}

// Validate a scope name.
static void
fkl_validate_scope_name(const pp_path::element &elem, const parse_location &loc)
{
	if (elem.name()[0] == '%') {
		throw pp_parse_error("invalid scope name: " + elem.name(), loc);
	}
}

//
// Start a new scope.
//
void
fkl_open_scope(const parse_location &loc,
               const string &name, const pp_binding_ptr &binding)
{
	DTRACE(TRACE_SCOPES, "open scope: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	try {
		pp_path::element elem(name);

		// make sure the name is valid
		fkl_validate_scope_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		// make a new scope and link it into the tree
		pp_scope_ptr tmp_scope = new_pp_scope(binding);
		pp_context new_ctxt(elem, tmp_scope);
		tmp_scope->set_parent(current_context.scope());

		// add the new scope to the parent
		current_context.add_dirent(elem, tmp_scope);

		// save the current scope, and set the new scope
		pp_context_push(new_ctxt);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}

//
// Close the current scope.
//
void
fkl_close_scope(const parse_location &loc)
{
	(void)loc;
	DTRACE(TRACE_SCOPES, "close scope: " + current_context.name());

	// retore the previous context
	pp_context_pop();
}

//
// Define a bookmark
//
void
fkl_bookmark(const parse_location &loc, const string &name)
{
	if (!lang_valid_bookmark_name(name)) {
		throw pp_parse_error("invalid bookmark name: " + name, loc);
	}
	current_context.add_bookmark(name);
}

// Validate a register name.
static void
fkl_validate_register_name(const pp_path::element &elem,
                           const parse_location &loc)
{
	if (elem.name()[0] != '%') {
		throw pp_parse_error("invalid register name: " + elem.name(),
		                     loc);
	}
}

//
// Define a register.
//
pp_register_ptr
fkl_reg(const parse_location &loc,
        const string &name, const pp_value &address, pp_bitwidth width)
{
	// check that we have a current binding
	const pp_binding_const_ptr &cur_binding = current_context.binding();
	DASSERT_MSG(cur_binding, "no binding for register " + name);
	return fkl_reg(loc, name, cur_binding, address, width);
}
pp_register_ptr
fkl_reg(const parse_location &loc,
        const string &name, const pp_binding_const_ptr &binding,
        const pp_value &address, pp_bitwidth width)
{
	DTRACE(TRACE_REGS, "reg: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	try {
		pp_path::element elem(name);

		// make sure the name is valid
		fkl_validate_register_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		pp_register_ptr reg_ptr = new_pp_bound_register(
				binding, address, width);
		current_context.add_dirent(elem, reg_ptr);
		return reg_ptr;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
pp_register_ptr
fkl_reg(const parse_location &loc,
        const pp_value &address, pp_bitwidth width)
{
	// check that we have a current binding
	const pp_binding_const_ptr &cur_binding = current_context.binding();
	DASSERT_MSG(cur_binding, "no binding for register");
	return fkl_reg(loc, cur_binding, address, width);
}
pp_register_ptr
fkl_reg(const parse_location &loc,
        const pp_binding_const_ptr &binding, const pp_value &address,
        pp_bitwidth width)
{
	(void)loc;
	return new_pp_bound_register(binding, address, width);
}


//
// Define a proc-register
//
pp_register_ptr
fkl_reg(const parse_location &loc,
        const string &name, const pp_rwprocs_ptr &access, pp_bitwidth width)
{
	DTRACE(TRACE_REGS, "reg: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	try {
		pp_path::element elem(name);

		// make sure the name is valid
		fkl_validate_register_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		pp_register_ptr reg_ptr = new_pp_proc_register(access, width);
		current_context.add_dirent(elem, reg_ptr);
		return reg_ptr;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
pp_register_ptr
fkl_reg(const parse_location &loc,
        const pp_rwprocs_ptr &access, pp_bitwidth width)
{
	(void)loc;
	return new_pp_proc_register(access, width);
}

//
// Define a regbits from a register name and bit range.
//
pp_regbits
fkl_bits(const parse_location &loc, const string &regname)
{
	const pp_register_const_ptr &reg = fkl_get_register(loc, regname);
	return fkl_bits(loc, reg);
}
pp_regbits
fkl_bits(const parse_location &loc, const string &regname, pp_bitwidth bit)
{
	const pp_register_const_ptr &reg = fkl_get_register(loc, regname);
	return fkl_bits(loc, reg, bit);
}
pp_regbits
fkl_bits(const parse_location &loc,
         const string &regname, pp_bitwidth hi_bit, pp_bitwidth lo_bit)
{
	const pp_register_const_ptr &reg = fkl_get_register(loc, regname);
	return fkl_bits(loc, reg, hi_bit, lo_bit);
}
pp_regbits
fkl_bits(const parse_location &loc, const pp_register_const_ptr &reg)
{
	(void)loc;
	return pp_regbits(reg, reg->width()-1, 0);
}
pp_regbits
fkl_bits(const parse_location &loc,
         const pp_register_const_ptr &reg, pp_bitwidth bit)
{
	(void)loc;
	return pp_regbits(reg, bit, bit);
}
pp_regbits
fkl_bits(const parse_location &loc,
         const pp_register_const_ptr &reg,
         pp_bitwidth hi_bit, pp_bitwidth lo_bit)
{
	(void)loc;
	return pp_regbits(reg, hi_bit, lo_bit);
}


// Validate a field name.
static void
fkl_validate_field_name(const pp_path::element &elem, const parse_location &loc)
{
	if (elem.name()[0] == '%') {
		throw pp_parse_error("invalid field name: " + elem.name(), loc);
	}
}

//
// Define a field as a set of register-bits.
//
pp_field_ptr
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype_const_ptr &type,
          const pp_regbits &bits)
{
	DTRACE(TRACE_FIELDS, "direct field: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);

	try {
		pp_path::element elem(name);

		// make sure the name is valid
		fkl_validate_field_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		// create a field and add it to the current scope
		pp_direct_field_ptr field_ptr = new_pp_direct_field(type, bits);
		current_context.add_dirent(elem, field_ptr);
		return field_ptr;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
pp_field_ptr
fkl_field(const parse_location &loc,
          const string &name, const string &type, const pp_regbits &bits)
{
	return fkl_field(loc, name, current_context.resolve_datatype(type),
	                 bits);
}


//
// Define a field as a constant value.
//
pp_field_ptr
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype_const_ptr &type,
          const pp_value &value)
{
	DTRACE(TRACE_FIELDS, "constant field: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);

	try {
		pp_path::element elem(name);

		// make sure the name is valid
		fkl_validate_field_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		// create a field and add it to the current scope
		pp_constant_field_ptr field_ptr =
				new_pp_constant_field(type, value);
		current_context.add_dirent(elem, field_ptr);
		return field_ptr;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
pp_field_ptr
fkl_field(const parse_location &loc,
          const string &name, const string &type, const pp_value &value)
{
	return fkl_field(loc, name, current_context.resolve_datatype(type),
	                 value);
}
//
// Define a field as a set of procedures.
//
pp_field_ptr
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype_const_ptr &type,
          const pp_rwprocs_ptr &access)
{
	DTRACE(TRACE_FIELDS, "proc field: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);
	DASSERT_MSG(access, "found NULL accessors for field " + name);

	try {
		pp_path::element elem(name);

		// make sure the name is valid
		fkl_validate_field_name(elem, loc);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		// create a field and add it to the current scope
		pp_proc_field_ptr field_ptr = new_pp_proc_field(type, access);
		current_context.add_dirent(elem, field_ptr);
		return field_ptr;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
pp_field_ptr
fkl_field(const parse_location &loc,
          const string &name, const string &type,
          const pp_rwprocs_ptr &access)
{
	return fkl_field(loc, name, current_context.resolve_datatype(type),
	                 access);
}

//
// Define an alias
//
pp_alias_ptr
fkl_alias(const parse_location &loc, const string &name, const string &target)
{
	DTRACE(TRACE_REGS, "alias: " + name + " -> " + target);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	try {
		pp_path::element elem(name);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		pp_path path = current_context.resolve_path(target);
		if (!path.is_initialized()) {
			throw pp_path::not_found_error(target);
		}
		pp_alias_ptr alias_ptr = new_pp_alias(path);
		current_context.add_dirent(elem, alias_ptr);
		return alias_ptr;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}

// Validate a datatype name.
void
fkl_validate_type_name(const string &name, const parse_location &loc)
{
	if (!lang_valid_datatype_name(name)) {
		throw pp_parse_error("invalid datatype name: " + name, loc);
	}
}

//
// Define a pp_int_datatype.
//
pp_datatype_ptr
fkl_int(const parse_location &loc, const string &name, const string &units)
{
	DTRACE(TRACE_TYPES, "int: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_int_datatype_ptr int_ptr = new_pp_int_datatype(units);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, int_ptr);
	}
	return int_ptr;
}

//
// Define a pp_hex_datatype.
//
pp_datatype_ptr
fkl_hex(const parse_location &loc,
        const string &name, const pp_bitwidth width, const string &units)
{
	DTRACE(TRACE_TYPES, "hex: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_hex_datatype_ptr hex_ptr = new_pp_hex_datatype(width, units);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, hex_ptr);
	}
	return hex_ptr;
}

//
// Define a pp_bitmask_datatype.
//
pp_datatype_ptr
fkl_bitmask(const parse_location &loc,
            const string &name, const fkl_kvpair_list &kvlist)
{
	DTRACE(TRACE_TYPES, "bitmask: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_bitmask_datatype_ptr bitmask_ptr = new_pp_bitmask_datatype(kvlist);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, bitmask_ptr);
	}
	return bitmask_ptr;
}

//
// Define a pp_string_datatype.
//
pp_datatype_ptr
fkl_string(const parse_location &loc, const string &name)
{
	DTRACE(TRACE_TYPES, "string: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_string_datatype_ptr string_ptr = new_pp_string_datatype();
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, string_ptr);
	}
	return string_ptr;
}

//
// Define a pp_enum_datatype.
//
pp_datatype_ptr
fkl_enum(const parse_location &loc,
         const string &name, const fkl_kvpair_list &kvlist)
{
	DTRACE(TRACE_TYPES, "enum: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_enum_datatype_ptr enum_ptr = new_pp_enum_datatype(kvlist);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, enum_ptr);
	}
	return enum_ptr;
}

//
// Define a pp_multi_datatype.
//
pp_datatype_ptr
fkl_multi(const parse_location &loc,
	      const string &name, const fkl_range_list &rangelist)
{
	DTRACE(TRACE_TYPES, "multi: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_multi_datatype_ptr multi_ptr =
		new_pp_multi_datatype();
	for (size_t i = 0; i < rangelist.size(); i++) {
		multi_ptr->add_range(rangelist[i].type,
				rangelist[i].low, rangelist[i].high);
	}
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, multi_ptr);
	}
	return multi_ptr;
}

//
// Define a pp_bool_datatype.
//
pp_datatype_ptr
fkl_bool(const parse_location &loc,
         const string &name, const string &true_str, const string &false_str)
{
	DTRACE(TRACE_TYPES, "bool: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_bool_datatype_ptr bool_ptr = new_pp_bool_datatype(
			true_str, false_str);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, bool_ptr);
	}
	return bool_ptr;
}

//
// Define a pp_fixed_datatype.
//
pp_datatype_ptr
fkl_fixed(const parse_location &loc,
          const string &name, int nbits, const string &units)
{
	DTRACE(TRACE_TYPES, "fixed: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_fixed_datatype_ptr fixed_ptr = new_pp_fixed_datatype(nbits, units);
	if (name != "") {
		fkl_validate_type_name(name, loc);
		current_context.add_datatype(name, fixed_ptr);
	}
	return fixed_ptr;
}

