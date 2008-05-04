/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 */

#include "pp.h"
#include "fake_language.h"
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
GET_DIRENT(const string &path)
{
	return current_context.lookup_dirent(path);
}

//
// Resolve a path to a field, relative to the current scope.
//
const pp_field *
GET_FIELD(const string &path)
{
	return current_context.lookup_field(path);
}

//
// Resolve a path to a register, relative to the current scope.
//
const pp_register *
GET_REGISTER(const string &path)
{
	// magic registers
	extern pp_register *magic_zeros;
	extern pp_register *magic_ones;

	if (path == "%0")
		return magic_zeros;
	if (path == "%1")
		return magic_ones;
	return current_context.lookup_register(path);
}

//
// Check whether a path resolves successfully.
//
bool
DEFINED(const string &path)
{
	return current_context.dirent_defined(path);
}

//
// Read and write a field or register.
//
pp_value
READ(const string &path)
{
	const pp_dirent *de = GET_DIRENT(path);
	if (de->is_register()) {
		return pp_register_from_dirent(de)->read();
	} else if (de->is_field()) {
		return pp_field_from_dirent(de)->read();
	}
	throw pp_dirent::conversion_error("path is not a register nor field: "
	    + to_string(path) + " (" + to_string(de->dirent_type()) + ")");
}
void
WRITE(const string &path, const pp_value &value)
{
	const pp_dirent *de = GET_DIRENT(path);
	if (de->is_register()) {
		pp_register_from_dirent(de)->write(value);
	} else if (de->is_field()) {
		pp_field_from_dirent(de)->write(value);
	}
	throw pp_dirent::conversion_error("path is not a register nor field: "
	    + to_string(path) + " (" + to_string(de->dirent_type()) + ")");
}

//
// Read and wite regbits.
//
pp_value
READ(const pp_regbits &bits)
{
	return bits.read();
}
void
WRITE(const pp_regbits &bits, const pp_value &value)
{
	bits.write(value);
}

//
// Start a new scope.
//
void
OPEN_SCOPE(const string &name, const pp_binding_ptr &binding)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_SCOPES, "scope: " + name);

	// note: this is not a debug-only test
	if (name != "" && DEFINED(name)) {
		PP_WARN("dirent redefined: " + name);
	}

	pp_scope_ptr tmp_scope = new_pp_scope(binding);
	pp_context new_ctxt(name, tmp_scope);

	// save the current scope
	tmp_scope->set_parent(current_context.scope());
	context_stack.push_back(current_context);

	// set current_context
	current_context = new_ctxt;
}

//
// Close the current scope.
//
void
CLOSE_SCOPE()
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_SCOPES, "end scope: " + current_context.name());
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
CLOSE_SCOPE(const string &new_name)
{
	DASSERT_MSG(!current_context.is_readonly(),
	    "current_context is read-only");
	// there had better be a current scope and a parent scope
	DASSERT_MSG(!context_stack.empty(), "invalid parent context");

	// note: this is not a debug-only test
	if (DEFINED("../" + new_name)) {
		PP_WARN("dirent redefined: " + new_name);
	}

	current_context.rename(new_name);
	CLOSE_SCOPE();
}

//
// Define a register.
//
void
REGN(const string &name, const pp_value &address, pp_bitwidth width)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_REGS, "reg: " + name);
	// enforce that registers start with '%'
	DASSERT_MSG(name[0] == '%', "register must start with %: " + name);
	const pp_binding *cur_binding = current_context.binding();
	// check that we have a current binding
	DASSERT_MSG(cur_binding, "no binding for register " + name);

	// note: this is not a debug-only test
	if (DEFINED(name)) {
		PP_WARN("register redefined: " + name);
	}

	pp_register_ptr reg_ptr = new_pp_register(
			cur_binding, address, width);
	current_context.add_dirent(name, reg_ptr);
}

//
// Define a regbits from a register name and bit range.
//
pp_regbits
BITS(const string &regname)
{
	const pp_register *reg = GET_REGISTER(regname);
	return pp_regbits(reg, reg->width()-1, 0);
}
pp_regbits
BITS(const string &regname, pp_bitwidth bit)
{
	const pp_register *reg = GET_REGISTER(regname);
	return pp_regbits(reg, bit, bit);
}
pp_regbits
BITS(const string &regname, pp_bitwidth hi_bit, pp_bitwidth lo_bit)
{
	const pp_register *reg = GET_REGISTER(regname);
	return pp_regbits(reg, hi_bit, lo_bit);
}

//
// Define a register and a field that consumes that register.
//
void
REGFIELDN(const string &name, const pp_value &address, const pp_datatype *type,
		pp_bitwidth width)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	string regname = "%" + name;
	REGN(regname, address, width);
	FIELD(name, type, BITS(regname, width-1, 0));
}
void
REGFIELDN(const string &name, const pp_value &address, const string &type,
		pp_bitwidth width)
{
	REGFIELDN(name, address, current_context.resolve_datatype(type), width);
}

//
// Define a field as a set of register-bits.
//
void
FIELD(const string &name, const pp_datatype *type, const pp_regbits &bits)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_FIELDS, "field: " + name);
	// sanity
	DASSERT_MSG(type, "found NULL pp_datatype for field " + name);

	// note: this is not a debug-only test
	if (DEFINED(name)) {
		PP_WARN("scope or field redefined: " + name);
	}

	// create a field and add it to the current scope
	pp_direct_field_ptr field_ptr = new_pp_direct_field(type, bits);
	current_context.add_dirent(name, field_ptr);
}
void
FIELD(const string &name, const string &type, const pp_regbits &bits)
{
	FIELD(name, current_context.resolve_datatype(type), bits);
}
//
// Define a field as a constant value.
//
void
FIELD(const string &name, const pp_datatype *type, const pp_value &value)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	pp_constant_field_ptr field_ptr = new_pp_constant_field(type, value);
	current_context.add_dirent(name, field_ptr);
}
void
FIELD(const string &name, const string &type, const pp_value &value)
{
	FIELD(name, current_context.resolve_datatype(type), value);
}

//
// Define a field as a set of procedures.
//
void
FIELD(const string &name, const pp_datatype *type,
		const proc_field_accessor_ptr &access)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	pp_proc_field_ptr field_ptr = new_pp_proc_field(type, access);
	current_context.add_dirent(name, field_ptr);
}
void
FIELD(const string &name, const string &type,
		const proc_field_accessor_ptr &access)
{
	FIELD(name, current_context.resolve_datatype(type), access);
}

//
// Define a pp_int datatype.
//
pp_int *
INT(const string &name, const string &units)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "int: " + name);

	pp_int_ptr int_ptr = new_pp_int(units);
	if (name == "") {
		current_context.add_datatype(int_ptr);
	} else {
		current_context.add_datatype(name, int_ptr);
	}
	return int_ptr.get();
}

//
// Define a pp_hex datatype.
//
pp_hex *
HEX(const string &name, const pp_bitwidth width, const string &units)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "hex: " + name);

	pp_hex_ptr hex_ptr = new_pp_hex(width, units);
	if (name == "") {
		current_context.add_datatype(hex_ptr);
	} else {
		current_context.add_datatype(name, hex_ptr);
	}
	return hex_ptr.get();
}

//
// Define a pp_bitmask datatype.
//
pp_bitmask *
BITMASK_(const string &name, const string &dflt,
    const pp_helper_kvpair_list &kvlist)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "bitmask: " + name);

	pp_bitmask_ptr bitmask_ptr = new_pp_bitmask(kvlist);
	if (dflt != "") {
		bitmask_ptr->set_default(dflt);
	}

	if (name == "") {
		current_context.add_datatype(bitmask_ptr);
	} else {
		current_context.add_datatype(name, bitmask_ptr);
	}

	return bitmask_ptr.get();
}

//
// Define a pp_enum datatype.
//
pp_enum *
ENUM_(const string &name, const pp_helper_kvpair_list &kvlist)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "enum: " + name);

	pp_enum_ptr enum_ptr = new_pp_enum(kvlist);
	if (name == "") {
		current_context.add_datatype(enum_ptr);
	} else {
		current_context.add_datatype(name, enum_ptr);
	}

	return enum_ptr.get();
}

//
// Define a pp_bool datatype.
//
pp_bool *
BOOL(const string &name, const string &true_str, const string &false_str)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "bool: " + name);

	pp_bool_ptr bool_ptr = new_pp_bool(true_str, false_str);
	if (name == "") {
		current_context.add_datatype(bool_ptr);
	} else {
		current_context.add_datatype(name, bool_ptr);
	}

	return bool_ptr.get();
}
