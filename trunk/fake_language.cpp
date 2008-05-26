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
#include "pp_scope.h"
#include "pp_datatypes.h"

#include <stdexcept>

//FIXME: need tests

//
// Resolve a path to a dirent, relative to the current scope.
//
const pp_dirent *
fkl_get_dirent(const parse_location &loc, const string &path_str)
{
	try {
		pp_path path(path_str);
		const pp_dirent *de = current_context.lookup_dirent(path);
		if (de == NULL) {
			throw pp_path::not_found_error(path_str);
		}
		return de;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}

//
// Resolve a path to a field, relative to the current scope.
//
const pp_field *
fkl_get_field(const parse_location &loc, const string &path_str)
{
	try {
		pp_path path(path_str);
		const pp_field *field = current_context.lookup_field(path);
		if (field == NULL) {
			throw pp_path::not_found_error(path_str);
		}
		return field;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}

//
// Resolve a path to a register, relative to the current scope.
//
const pp_register *
fkl_get_register(const parse_location &loc, const string &path_str)
{
	// magic registers
	extern pp_register *magic_zeros;
	extern pp_register *magic_ones;

	if (path_str == "%0")
		return magic_zeros;
	if (path_str == "%1")
		return magic_ones;

	try {
		pp_path path(path_str);
		const pp_register *reg = current_context.lookup_register(path);
		if (reg == NULL) {
			throw pp_path::not_found_error(path_str);
		}
		return reg;
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
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
	return current_context.dirent_defined(path);
}

//
// Read and write a field or register.
//
pp_value
fkl_read(const parse_location &loc, const string &path_str)
{
	try {
		const pp_dirent *de = fkl_get_dirent(loc, path_str);
		if (de->is_register()) {
			return pp_register_from_dirent(de)->read();
		} else if (de->is_field()) {
			return pp_field_from_dirent(de)->read();
		}
		throw pp_dirent::conversion_error(
		    "path is not a register or field: "
		    + path_str + " (" + to_string(de->dirent_type()) + ")");
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
void
fkl_write(const parse_location &loc,
          const string &path_str, const pp_value &value)
{
	try {
		const pp_dirent *de = GET_DIRENT(path_str);
		if (de->is_register()) {
			pp_register_from_dirent(de)->write(value);
		} else if (de->is_field()) {
			pp_field_from_dirent(de)->write(value);
		}
		throw pp_dirent::conversion_error(
		    "path is not a register or field: "
		    + path_str + " (" + to_string(de->dirent_type()) + ")");
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
	return bits.read();
}
void
fkl_write(const parse_location &loc,
          const pp_regbits &bits, const pp_value &value)
{
	bits.write(value);
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

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		pp_scope_ptr tmp_scope = new_pp_scope(binding);
		pp_context new_ctxt(elem, tmp_scope);

		// save the current scope
		tmp_scope->set_parent(current_context.scope());
		context_stack.push_back(current_context);

		// set current_context
		current_context = new_ctxt;
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
	DTRACE(TRACE_SCOPES, "close scope: " + current_context.name());

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	// there had better be a current scope and a parent scope
	DASSERT_MSG(!context_stack.empty(), "invalid parent context");

	// add the current scope to the parent context
	pp_context old_ctxt = context_stack.back();
	old_ctxt.add_dirent(current_context);

	// restore the parent context
	current_context = old_ctxt;
	context_stack.pop_back();
}
void
fkl_close_scope(const parse_location &loc, const string &new_name)
{
	DTRACE(TRACE_SCOPES, "close+rename scope: " + new_name);

	DASSERT_MSG(!current_context.is_readonly(),
	    "current_context is read-only");

	// there had better be a current scope and a parent scope
	DASSERT_MSG(!context_stack.empty(), "invalid parent context");

	try {
		pp_path::element elem(new_name);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path("..", elem))) {
			PP_WARN(to_string(loc)
					+ ": '" + new_name + "' redefined");
		}

		current_context.rename(elem);
		fkl_close_scope(loc);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
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

//
// Define a register.
//
void
fkl_regn(const parse_location &loc,
         const string &name, const pp_value &address, pp_bitwidth width)
{
	DTRACE(TRACE_REGS, "reg: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	// enforce that registers start with '%'
	DASSERT_MSG(name[0] == '%', "register must start with %: " + name);

	// check that we have a current binding
	const pp_binding *cur_binding = current_context.binding();
	DASSERT_MSG(cur_binding, "no binding for register " + name);

	try {
		pp_path::element elem(name);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		pp_register_ptr reg_ptr = new_pp_register(
				cur_binding, address, width);
		current_context.add_dirent(elem, reg_ptr);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}

//
// Define a regbits from a register name and bit range.
//
pp_regbits
fkl_bits(const parse_location &loc, const string &regname)
{
	const pp_register *reg = fkl_get_register(loc, regname);
	return pp_regbits(reg, reg->width()-1, 0);
}
pp_regbits
fkl_bits(const parse_location &loc, const string &regname, pp_bitwidth bit)
{
	const pp_register *reg = fkl_get_register(loc, regname);
	return pp_regbits(reg, bit, bit);
}
pp_regbits
fkl_bits(const parse_location &loc,
         const string &regname, pp_bitwidth hi_bit, pp_bitwidth lo_bit)
{
	const pp_register *reg = fkl_get_register(loc, regname);
	return pp_regbits(reg, hi_bit, lo_bit);
}

//
// Define a register and a field that consumes that register.
//
void
fkl_regfieldn(const parse_location &loc,
              const string &name, const pp_value &address,
              const pp_datatype *type, pp_bitwidth width)
{
	DTRACE(TRACE_FIELDS | TRACE_REGS, "regfield: " + name);

	// create a register and a field, sanity checking is done within
	string regname = "%" + name;
	fkl_regn(loc, regname, address, width);
	fkl_field(loc, name, type, fkl_bits(loc, regname, width-1, 0));
}
void
fkl_regfieldn(const parse_location &loc,
              const string &name, const pp_value &address,
              const string &type, pp_bitwidth width)
{
	fkl_regfieldn(loc, name, address,
			current_context.resolve_datatype(type), width);
}

//
// Define a field as a set of register-bits.
//
void
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype *type, const pp_regbits &bits)
{
	DTRACE(TRACE_FIELDS, "direct field: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);

	try {
		pp_path::element elem(name);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		// create a field and add it to the current scope
		pp_direct_field_ptr field_ptr = new_pp_direct_field(type, bits);
		current_context.add_dirent(elem, field_ptr);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
void
fkl_field(const parse_location &loc,
          const string &name, const string &type, const pp_regbits &bits)
{
	fkl_field(loc, name, current_context.resolve_datatype(type), bits);
}
//
// Define a field as a constant value.
//
void
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype *type, const pp_value &value)
{
	DTRACE(TRACE_FIELDS, "constant field: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);

	try {
		pp_path::element elem(name);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		// create a field and add it to the current scope
		pp_constant_field_ptr field_ptr =
				new_pp_constant_field(type, value);
		current_context.add_dirent(elem, field_ptr);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
void
fkl_field(const parse_location &loc,
          const string &name, const string &type, const pp_value &value)
{
	fkl_field(loc, name, current_context.resolve_datatype(type), value);
}
//
// Define a field as a set of procedures.
//
void
fkl_field(const parse_location &loc,
          const string &name, const pp_datatype *type,
          const proc_field_accessor_ptr &access)
{
	DTRACE(TRACE_FIELDS, "proc field: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	DASSERT_MSG(type, "found NULL datatype for field " + name);
	DASSERT_MSG(access, "found NULL accessors for field " + name);

	try {
		pp_path::element elem(name);

		// note: this is not a debug-only test
		if (fkl_defined(loc, pp_path(elem))) {
			PP_WARN(to_string(loc) + ": '" + name + "' redefined");
		}

		// create a field and add it to the current scope
		pp_proc_field_ptr field_ptr = new_pp_proc_field(type, access);
		current_context.add_dirent(elem, field_ptr);
	} catch (pp_path::invalid_error &e) {
		throw pp_parse_error(e.what(), loc);
	}
}
void
fkl_field(const parse_location &loc,
          const string &name, const string &type,
          const proc_field_accessor_ptr &access)
{
	fkl_field(loc, name, current_context.resolve_datatype(type), access);
}

static void
validate_type_name(const string &name, const parse_location &loc)
{
	if (!lang_valid_datatype_name(name)) {
		throw pp_parse_error("invalid datatype name: " + name, loc);
	}
}

//
// Define a pp_int datatype.
//
pp_int *
fkl_int(const parse_location &loc, const string &name, const string &units)
{
	DTRACE(TRACE_TYPES, "int: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_int_ptr int_ptr = new_pp_int(units);
	if (name == "") {
		current_context.add_datatype(int_ptr);
	} else {
		validate_type_name(name, loc);
		current_context.add_datatype(name, int_ptr);
	}
	return int_ptr.get();
}

//
// Define a pp_hex datatype.
//
pp_hex *
fkl_hex(const parse_location &loc,
        const string &name, const pp_bitwidth width, const string &units)
{
	DTRACE(TRACE_TYPES, "hex: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	pp_hex_ptr hex_ptr = new_pp_hex(width, units);
	if (name == "") {
		current_context.add_datatype(hex_ptr);
	} else {
		validate_type_name(name, loc);
		current_context.add_datatype(name, hex_ptr);
	}
	return hex_ptr.get();
}

//
// Define a pp_bitmask datatype.
//
pp_bitmask *
fkl_bitmask(const parse_location &loc,
            const string &name, const fkl_kvpair_list &kvlist)
{
	DTRACE(TRACE_TYPES, "bitmask: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	for (size_t i = 0; i < kvlist.size(); i++) {
		const string &key = kvlist.key_at(i);
		if (!lang_valid_datatype_key(key)) {
			throw pp_parse_error(
			    "invalid bitmask key: " + key, loc);
		}
	}

	pp_bitmask_ptr bitmask_ptr = new_pp_bitmask(kvlist);
	if (name == "") {
		current_context.add_datatype(bitmask_ptr);
	} else {
		validate_type_name(name, loc);
		current_context.add_datatype(name, bitmask_ptr);
	}
	return bitmask_ptr.get();
}

//
// Define a pp_enum datatype.
//
pp_enum *
fkl_enum(const parse_location &loc,
         const string &name, const fkl_kvpair_list &kvlist)
{
	DTRACE(TRACE_TYPES, "enum: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	for (size_t i = 0; i < kvlist.size(); i++) {
		const string &key = kvlist.key_at(i);
		if (!lang_valid_datatype_key(key)) {
			throw pp_parse_error(
			    "invalid enum key: " + key, loc);
		}
	}

	pp_enum_ptr enum_ptr = new_pp_enum(kvlist);
	if (name == "") {
		current_context.add_datatype(enum_ptr);
	} else {
		validate_type_name(name, loc);
		current_context.add_datatype(name, enum_ptr);
	}
	return enum_ptr.get();
}

//
// Define a pp_bool datatype.
//
pp_bool *
fkl_bool(const parse_location &loc,
         const string &name, const string &true_str, const string &false_str)
{
	DTRACE(TRACE_TYPES, "bool: " + name);

	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");

	if (!lang_valid_datatype_key(true_str)) {
		throw pp_parse_error("invalid bool key: " + true_str, loc);
	}
	if (!lang_valid_datatype_key(false_str)) {
		throw pp_parse_error("invalid bool key: " + false_str, loc);
	}

	pp_bool_ptr bool_ptr = new_pp_bool(true_str, false_str);
	if (name == "") {
		current_context.add_datatype(bool_ptr);
	} else {
		validate_type_name(name, loc);
		current_context.add_datatype(name, bool_ptr);
	}
	return bool_ptr.get();
}
