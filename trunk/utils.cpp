/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * Implementation of functions contained in utils.h
 */

#include "pp.h"
#include "utils.h"
#include "pp_field.h"
#include "pp_fields.h"
#include "pp_path.h"
#include "pp_dirent.h"
#include "pp_register.h"
#include "pp_scope.h"
#include "pp_datatypes.h"
#include "device_init.h"

#include <stdexcept>
using namespace std;

/*
 * get_field()
 *
 * A function to search the tree and return a pointer to
 * the field in question.
 */
const pp_field *
get_field(const pp_scope *scope, const pp_path &path)
{
	const pp_dirent *de = get_dirent(scope, path);
	if (de->dirent_type() == PP_DIRENT_FIELD) {
		return pp_field_from_dirent(de);
	}
	/* default error case */
	throw std::out_of_range("path is not a field: "
			+ to_string(path));
}

/*
 * get_register()
 *
 * A function to search the tree and return a pointer to
 * the field in question.
 */
const pp_register *
get_register(const pp_scope *scope, const pp_path &path)
{
	const pp_dirent *de = get_dirent(scope, path);
	if (de->dirent_type() == PP_DIRENT_REGISTER) {
		return pp_register_from_dirent(de);
	}
	/* default error case */
	throw std::out_of_range("path is not a register: "
			+ to_string(path));
}

/*
 * get_dirent()
 *
 * A function to search the tree and return a pointer to
 * the dirent in question.
 *
 * NOTE: this takes path as a non-const reference, and may modify it.
 */
static const pp_dirent *
get_dirent_internal(const pp_scope *scope, pp_path &path)
{
	/* error case */
	if (path.empty()) {
		throw std::out_of_range("path not found: "
			+ to_string(path));
	}

	/* grab first element of path */
	string path_front = path.pop_front();

	/*
	 * Look up the dirent of the next element.  This can throw
	 * std::out_of_range if the dirent does not exist.
	 */
	const pp_dirent *de;
	if (path_front == "..") {
		de = scope->parent();
	} else {
		de = scope->dirent(path_front);
	}

	/* did we find the dirent? */
	if (path.empty()) {
		return de;
	}

	/* keep looking */
	if (de->dirent_type() == PP_DIRENT_SCOPE) {
		return get_dirent(pp_scope_from_dirent(de), path);
	}

	/* default error case */
	throw std::out_of_range("path is not a known dirent_type: "
			+ to_string(path)
			+ "(" + to_string(de->dirent_type()) + ")");
}
/*
 * NOTE: this takes a copy of the path, since it will be modified.
 */
const pp_dirent *
get_dirent(const pp_scope *scope, pp_path path)
{
	if (path.front() == "^") {
		while (!scope->is_root() && !scope->binding()) {
			scope = scope->parent();
		}
		path.pop_front();
	}
	return get_dirent_internal(scope, path);
}

/*
 * dirent_defined()
 *
 * Tests whether the pp_path resolves to a defined dirent.
 */
bool
dirent_defined(const pp_scope *scope, const pp_path &path)
{
	if (!scope) {
		return false;
	}

	const pp_dirent *d = NULL;
	try {
		d = get_dirent(scope, path);
	} catch (std::out_of_range &e) {
	}

	return (d != NULL);
}

//FIXME: need tests

static pp_scope_ptr cur_scope;
static std::vector<pp_scope_ptr> scope_stack;

static string cur_scope_name;
static std::vector<string> scope_name_stack;

static pp_const_binding_ptr cur_binding;
static std::vector<pp_const_binding_ptr> binding_stack;

/*
 * Resolve a path to a field, relative to the current scope.  If the specified
 * path does not exist or is not a field, throw std::out_of_range.
 */
const pp_field *
GET_FIELD(const pp_path &path)
{
	DASSERT_MSG(cur_scope, "found NULL cur_scope");
	return get_field(cur_scope.get(), path);
}

/*
 * Resolve a path to a register, relative to the current scope.  If the
 * specified path does not exist or is not a register, throw
 * std::out_of_range.
 */
const pp_register *
GET_REGISTER(const pp_path &path)
{
	DASSERT_MSG(cur_scope, "found NULL cur_scope");
	if (path == "0")
		return magic_zeros;
	if (path == "1")
		return magic_ones;
	return get_register(cur_scope.get(), path);
}

/*
 * Check whether a path resolves successfully.
 */
bool
DEFINED(const pp_path &path)
{
	return dirent_defined(cur_scope.get(), path);
}

/*
 * Start a new platform scope.  A pltform scope is the top-level scope in the
 * hierarchy.
 */
pp_scope_ptr
NEW_PLATFORM()
{
	// platform is the top level, cur_scope must not exist
	DASSERT_MSG(!cur_scope, "found non-NULL cur_scope");
	DASSERT_MSG(scope_stack.empty(), "scope_stack must be empty");

	OPEN_SCOPE("");
	global_datatypes_init(cur_scope.get());

	// FIXME: take these out when we have a real language
	pci_datatypes_init();

	DASSERT_MSG(cur_scope, "found NULL cur_scope");
	return cur_scope;
}

/*
 * Start a new scope.
 */
void
OPEN_SCOPE(const string &name, pp_const_binding_ptr binding)
{
	DTRACE(TRACE_SCOPES, "scope: " + name);

	// note: this is not a debug-only test
	if (DEFINED(name)) {
		WARN("scope or field redefined: " + name);
	}

	pp_scope_ptr tmp_scope_ = new_pp_scope(binding);

	// if we are not opening a top-level scope, save the current scope
	if (cur_scope) {
		tmp_scope_->set_parent(cur_scope.get());
		scope_stack.push_back(cur_scope);
		scope_name_stack.push_back(cur_scope_name);
	}
	// set cur_scope to the new scope
	cur_scope = tmp_scope_;
	cur_scope_name = name;

	// if we are starting a new binding, save the old binding
	if (binding) {
		binding_stack.push_back(cur_binding);
		cur_binding = binding;
	}
}

/*
 * Close the current scope.
 */
void
CLOSE_SCOPE()
{
	DTRACE(TRACE_SCOPES, "end scope: " + cur_scope_name);
	// there had better be a current scope and a parent scope
	DASSERT_MSG(cur_scope, "found NULL cur_scope");
	DASSERT_MSG(scope_stack.back(), "no parent scope");

	// add the current scope to the parent
	pp_scope_ptr parent = scope_stack.back();
	parent->add_dirent(cur_scope_name, cur_scope);

	// if the current scope is bound, restore the previous binding
	if (cur_scope->binding()) {
		cur_binding = binding_stack.back();
		binding_stack.pop_back();
	}

	// restore the parent scope and scope_name
	cur_scope = scope_stack.back();
	scope_stack.pop_back();
	cur_scope_name = scope_name_stack.back();
	scope_name_stack.pop_back();
}

/*
 * Define a register.
 */
void
REGN(const string &name, const pp_regaddr &address, pp_bitwidth width)
{
	DTRACE(TRACE_REGS, "reg: " + name);
	// check that we have a current binding
	DASSERT_MSG(cur_binding, "no binding for register " + name);
	// enforce that registers start with '%'
	DASSERT_MSG(name[0] == '%', "register must start with %: " + name);

	// note: this is not a debug-only test
	if (DEFINED(name)) {
		WARN("register redefined: " + name);
	}

	pp_register_ptr reg_ptr = new_pp_register(
			cur_binding.get(), address, width);
	cur_scope->add_dirent(name, reg_ptr);
}

/*
 * Define a simple regbits_field from a single range of bits from a single
 * register.
 */
void
SIMPLE_FIELD(const string &name, const pp_datatype *type,
		const string &regname, unsigned hi_bit, unsigned lo_bit)
{
	COMPLEX_FIELD(name, type, BITS(regname.c_str(), hi_bit, lo_bit));
}
void
SIMPLE_FIELD(const string &name, const string &type_str,
		const string &regname, unsigned hi_bit, unsigned lo_bit)
{
	const pp_datatype *type = cur_scope->resolve_datatype(type_str);
	SIMPLE_FIELD(name, type, regname, hi_bit, lo_bit);
}

/*
 * Define a regbits_field from an array of bitranges.
 *
 * ASSUMPTION: bit ranges are added sequentially from lowest to highest.
 */
static int
add_bitrange(pp_regbits_field *field, const reg_bitrange &bits, int ttl_bits)
{
	DTRACE(TRACE_FIELDS && TRACE_FIELD_BITS, string("  ")
			+ bits.regname + "["
			+ to_string(bits.hi_bit) + ","
			+ to_string(bits.lo_bit) + "]");

	const pp_register *reg = GET_REGISTER(bits.regname);
	int nbits = (bits.hi_bit - bits.lo_bit) + 1;

	// sanity check the bit range
	if (bits.hi_bit < bits.lo_bit
	 || bits.hi_bit >= (unsigned)reg->width()) {
		WARN("bad bit range: ["
			+ to_string(bits.hi_bit)
			+ string(":")
			+ to_string(bits.lo_bit)
			+ string("]"));
	}

	// add the current bitrange at the next free bit number
	field->add_regbits(reg, bits.lo_bit, PP_MASK(nbits), ttl_bits);
	return nbits;
}

void
COMPLEX_FIELD(const string &name, const pp_datatype *type,
		const reg_bitrange &bits0, const reg_bitrange &bits1,
		const reg_bitrange &bits2, const reg_bitrange &bits3,
		const reg_bitrange &bits4)
{
	DTRACE(TRACE_FIELDS, "field: " + name);
	// sanity
	DASSERT_MSG(type, "found NULL pp_datatype for field " + name);

	// note: this is not a debug-only test
	if (DEFINED(name)) {
		WARN("scope or field redefined: " + name);
	}

	// create a field and add each bitrange
	pp_regbits_field_ptr field_ptr = new_pp_regbits_field(type);
	int ttl_bits = 0;
	ttl_bits += add_bitrange(field_ptr.get(), bits0, ttl_bits);

	#define ADD_BITS(bits) do { \
		if (bits.regname == "") goto done_adding_bits; \
		ttl_bits += add_bitrange(field_ptr.get(), bits, ttl_bits); \
	} while (0)
	ADD_BITS(bits1); ADD_BITS(bits2); ADD_BITS(bits3); ADD_BITS(bits4);
done_adding_bits:

	cur_scope->add_dirent(name, field_ptr);
}
void
COMPLEX_FIELD(const string &name, const string &type,
		const reg_bitrange &bits0, const reg_bitrange &bits1,
		const reg_bitrange &bits2, const reg_bitrange &bits3,
		const reg_bitrange &bits4)
{
	COMPLEX_FIELD(name, cur_scope->resolve_datatype(type),
			bits0, bits1, bits2, bits3, bits4);
}

/*
 * Define a register and a field that consumes that register.
 */
void
REGFIELDN(const string &name, const pp_regaddr &address, const pp_datatype *type,
		pp_bitwidth width)
{
	string regname = "%" + name;
	REGN(regname, address, width);
	SIMPLE_FIELD(name, type, regname, width-1, 0);
}
void
REGFIELDN(const string &name, const pp_regaddr &address, const string &type,
		pp_bitwidth width)
{
	REGFIELDN(name, address, cur_scope->resolve_datatype(type), width);
}

/*
 * Define a constant-value field.
 */
void
CONSTANT_FIELD(const string &name, const pp_datatype *type,
		const pp_value &value)
{
	pp_constant_field_ptr field_ptr = new_pp_constant_field(type, value);
	cur_scope->add_dirent(name, field_ptr);
}
void
CONSTANT_FIELD(const string &name, const string &type, const pp_value &value)
{
	CONSTANT_FIELD(name, cur_scope->resolve_datatype(type), value);
}


/*
 * Define a pp_int datatype.
 */
pp_int *
INT(const string &name, const string &units)
{
	DTRACE(TRACE_TYPES, "int: " + name);
	// sanity
	DASSERT_MSG(cur_scope, "found NULL cur_scope");

	pp_int_ptr int_ptr = new_pp_int(units);
	if (name == "") {
		cur_scope->add_datatype(int_ptr);
	} else {
		cur_scope->add_datatype(name, int_ptr);
	}
	return int_ptr.get();
}

// see comment in utils.h
pp_bitmask *
BITMASK_(const string &name, const string &dflt,
	const kv_pair &kv0, const kv_pair &kv1,
	const kv_pair &kv2, const kv_pair &kv3,
	const kv_pair &kv4, const kv_pair &kv5,
	const kv_pair &kv6, const kv_pair &kv7,
	const kv_pair &kv8, const kv_pair &kv9,
	const kv_pair &kv10, const kv_pair &kv11,
	const kv_pair &kv12, const kv_pair &kv13,
	const kv_pair &kv14, const kv_pair &kv15,
	const kv_pair &kv16, const kv_pair &kv17,
	const kv_pair &kv18, const kv_pair &kv19,
	const kv_pair &kv20, const kv_pair &kv21,
	const kv_pair &kv22, const kv_pair &kv23,
	const kv_pair &kv24, const kv_pair &kv25,
	const kv_pair &kv26, const kv_pair &kv27,
	const kv_pair &kv28, const kv_pair &kv29,
	const kv_pair &kv30, const kv_pair &kv31)
{
	DTRACE(TRACE_TYPES, "bitmask: " + name);
	// sanity
	DASSERT_MSG(cur_scope, "found NULL cur_scope");

	pp_bitmask_ptr bitmask_ptr = new_pp_bitmask();
	if (dflt != "") {
		bitmask_ptr->set_default(dflt);
	}
	bitmask_ptr->add_bit(kv0.key, kv0.value);

	#define ADD_BMSK_KV(kv) do { \
		if (kv.key == "") goto done_adding_kv; \
		bitmask_ptr->add_bit(kv.key, kv.value); \
	} while (0)
	ADD_BMSK_KV(kv1); ADD_BMSK_KV(kv2); ADD_BMSK_KV(kv3);
	ADD_BMSK_KV(kv4); ADD_BMSK_KV(kv5); ADD_BMSK_KV(kv6);
	ADD_BMSK_KV(kv7); ADD_BMSK_KV(kv8); ADD_BMSK_KV(kv9);
	ADD_BMSK_KV(kv10); ADD_BMSK_KV(kv11); ADD_BMSK_KV(kv12);
	ADD_BMSK_KV(kv13); ADD_BMSK_KV(kv14); ADD_BMSK_KV(kv15);
	ADD_BMSK_KV(kv16); ADD_BMSK_KV(kv17); ADD_BMSK_KV(kv18);
	ADD_BMSK_KV(kv19); ADD_BMSK_KV(kv20); ADD_BMSK_KV(kv21);
	ADD_BMSK_KV(kv22); ADD_BMSK_KV(kv23); ADD_BMSK_KV(kv24);
	ADD_BMSK_KV(kv25); ADD_BMSK_KV(kv26); ADD_BMSK_KV(kv27);
	ADD_BMSK_KV(kv28); ADD_BMSK_KV(kv29); ADD_BMSK_KV(kv30);
	ADD_BMSK_KV(kv31);
done_adding_kv:

	if (name == "") {
		cur_scope->add_datatype(bitmask_ptr);
	} else {
		cur_scope->add_datatype(name, bitmask_ptr);
	}

	return bitmask_ptr.get();
}

// see comment in utils.h
pp_enum *
ENUM(const string &name,
	const kv_pair &kv0, const kv_pair &kv1,
	const kv_pair &kv2, const kv_pair &kv3,
	const kv_pair &kv4, const kv_pair &kv5,
	const kv_pair &kv6, const kv_pair &kv7,
	const kv_pair &kv8, const kv_pair &kv9,
	const kv_pair &kv10, const kv_pair &kv11,
	const kv_pair &kv12, const kv_pair &kv13,
	const kv_pair &kv14, const kv_pair &kv15,
	const kv_pair &kv16, const kv_pair &kv17,
	const kv_pair &kv18, const kv_pair &kv19)
{
	DTRACE(TRACE_TYPES, "enum: " + name);
	// sanity
	DASSERT_MSG(cur_scope, "found NULL cur_scope");

	pp_enum_ptr enum_ptr = new_pp_enum();
	enum_ptr->add_value(kv0.key, kv0.value);

	#define ADD_ENUM_KV(kv) do { \
		if (kv.key == "") goto done_adding_kv; \
		enum_ptr->add_value(kv.key, kv.value); \
	} while (0)
	ADD_ENUM_KV(kv1); ADD_ENUM_KV(kv2); ADD_ENUM_KV(kv3);
	ADD_ENUM_KV(kv4); ADD_ENUM_KV(kv5); ADD_ENUM_KV(kv6);
	ADD_ENUM_KV(kv7); ADD_ENUM_KV(kv8); ADD_ENUM_KV(kv9);
	ADD_ENUM_KV(kv10); ADD_ENUM_KV(kv11); ADD_ENUM_KV(kv12);
	ADD_ENUM_KV(kv13); ADD_ENUM_KV(kv14); ADD_ENUM_KV(kv15);
	ADD_ENUM_KV(kv16); ADD_ENUM_KV(kv17); ADD_ENUM_KV(kv18);
	ADD_ENUM_KV(kv19);
done_adding_kv:

	if (name == "") {
		cur_scope->add_datatype(enum_ptr);
	} else {
		cur_scope->add_datatype(name, enum_ptr);
	}

	return enum_ptr.get();
}

pp_bool *
BOOL(const string &name, const string &true_str, const string &false_str)
{
	DTRACE(TRACE_TYPES, "bool: " + name);
	// sanity
	DASSERT_MSG(cur_scope, "found NULL cur_scope");

	pp_bool_ptr bool_ptr = new_pp_bool(true_str, false_str);
	if (name == "") {
		cur_scope->add_datatype(bool_ptr);
	} else {
		cur_scope->add_datatype(name, bool_ptr);
	}

	return bool_ptr.get();
}
