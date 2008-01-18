/*
 * Copyright 2007 Google Inc. All Rights Reserved.
 * Author: lesleyn@google.com (Lesley Northam)
 *
 * Implementation of functions contained in utils.h
 */

#include "pp.h"
#include "utils.h"
#include "pp_context.h"
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

//FIXME: need tests

/*
 * These comprise the current context within the PP tree.
 */
static pp_context current_context;
static std::vector<pp_context> context_stack;

/*
 * This is a transaction-management helper.
 */
class pp_saved_context_impl
{
    private:
	pp_context m_context;

    public:
	pp_saved_context_impl(const pp_context &context)
	    : m_context(context)
	{
	}
	~pp_saved_context_impl()
	{
		DTRACE(TRACE_SCOPES, "restoring scope: " + m_context.name());
		current_context = m_context;
	}
};

// get a read-only copy of the current context
pp_context
GET_CURRENT_CONTEXT()
{
	return current_context.snapshot();
}

// when this saved_context expires, the context will be restored
pp_saved_context
SET_CURRENT_CONTEXT(const pp_context &new_ctxt)
{
	DTRACE(TRACE_SCOPES, "setting scope: " + new_ctxt.name());
	pp_saved_context old_ctxt(new pp_saved_context_impl(current_context));
	current_context = new_ctxt;
	return old_ctxt;
}


/*
 * Resolve a path to a field, relative to the current scope.  If the specified
 * path does not exist or is not a field, throw std::out_of_range.
 */
const pp_field *
GET_FIELD(const pp_path &path)
{
	DASSERT_MSG(current_context.is_valid(), "invalid current_context");
	return current_context.lookup_field(path);
}

/*
 * Resolve a path to a register, relative to the current scope.  If the
 * specified path does not exist or is not a register, throw
 * std::out_of_range.
 */
const pp_register *
GET_REGISTER(const pp_path &path)
{
	DASSERT_MSG(current_context.is_valid(), "invalid current_context");
	if (path == "%0")
		return magic_zeros;
	if (path == "%1")
		return magic_ones;
	return current_context.lookup_register(path);
}

/*
 * Check whether a path resolves successfully.
 */
bool
DEFINED(const pp_path &path)
{
	if (!current_context.is_valid()) {
		return false;
	}

	return current_context.dirent_defined(path);
}

/*
 * Start a new platform scope.  A pltform scope is the top-level scope in the
 * hierarchy.
 */
pp_scope *
NEW_PLATFORM()
{
	// platform is the top level, cur_scope must not exist
	DASSERT_MSG(!current_context.is_valid(),
		"current_context is already valid");
	DASSERT_MSG(context_stack.empty(), "context_stack must be empty");

	OPEN_SCOPE("");
	global_datatypes_init(current_context.scope());

	// FIXME: take these out when we have a real language
	pci_datatypes_init();

	DASSERT_MSG(current_context.is_valid(), "invalid current_context");
	return current_context.scope();
}

/*
 * Start a new scope.
 */
void
OPEN_SCOPE(const string &name, pp_const_binding_ptr binding)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_SCOPES, "scope: " + name);

	// note: this is not a debug-only test
	if (DEFINED(name)) {
		WARN("dirent redefined: " + name);
	}

	pp_scope_ptr tmp_scope = new_pp_scope(binding);
	pp_context new_ctxt(name, tmp_scope);

	// if we are not opening a top-level scope, save the current scope
	if (current_context.is_valid()) {
		tmp_scope->set_parent(current_context.scope());
		context_stack.push_back(current_context);
	}
	// set current_context
	current_context = new_ctxt;
}

/*
 * Close the current scope.
 */
void
CLOSE_SCOPE()
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_SCOPES, "end scope: " + current_context.name());
	// there had better be a current scope and a parent scope
	DASSERT_MSG(current_context.is_valid(), "invalid current_context");
	DASSERT_MSG(!context_stack.empty() && context_stack.back().is_valid(),
		"invalid parent context");

	// add the current scope to the parent context
	pp_context old_ctxt = context_stack.back();
	old_ctxt.add_dirent(current_context);

	// restore the parent context
	current_context = old_ctxt;
	context_stack.pop_back();
}

/*
 * Define a regbits from a register name and bit range.
 */
pp_regbits
BITS(const string &regname)
{
	const pp_register *reg = GET_REGISTER(regname);
	return pp_regbits(reg, reg->width()-1, 0);
}
pp_regbits
BITS(const string &regname, unsigned bit)
{
	const pp_register *reg = GET_REGISTER(regname);
	return pp_regbits(reg, bit, bit);
}
pp_regbits
BITS(const string &regname, unsigned hi_bit, unsigned lo_bit)
{
	const pp_register *reg = GET_REGISTER(regname);
	return pp_regbits(reg, hi_bit, lo_bit);
}

/*
 * Define a register.
 */
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
		WARN("register redefined: " + name);
	}

	pp_register_ptr reg_ptr = new_pp_register(
			cur_binding, address, width);
	current_context.add_dirent(name, reg_ptr);
}

/*
 * Define a simple direct_field from a single range of bits from a single
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
	const pp_datatype *type = current_context.resolve_datatype(type_str);
	SIMPLE_FIELD(name, type, regname, hi_bit, lo_bit);
}

void
COMPLEX_FIELD(const string &name, const pp_datatype *type,
		const pp_regbits &bits)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_FIELDS, "field: " + name);
	// sanity
	DASSERT_MSG(type, "found NULL pp_datatype for field " + name);

	// note: this is not a debug-only test
	if (DEFINED(name)) {
		WARN("scope or field redefined: " + name);
	}

	// create a field and add it to the current scope
	pp_direct_field_ptr field_ptr = new_pp_direct_field(type, bits);
	current_context.add_dirent(name, field_ptr);
}
void
COMPLEX_FIELD(const string &name, const string &type,
		const pp_regbits &bits)
{
	COMPLEX_FIELD(name, current_context.resolve_datatype(type), bits);
}

/*
 * Define a register and a field that consumes that register.
 */
void
REGFIELDN(const string &name, const pp_value &address, const pp_datatype *type,
		pp_bitwidth width)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	string regname = "%" + name;
	REGN(regname, address, width);
	SIMPLE_FIELD(name, type, regname, width-1, 0);
}
void
REGFIELDN(const string &name, const pp_value &address, const string &type,
		pp_bitwidth width)
{
	REGFIELDN(name, address, current_context.resolve_datatype(type), width);
}

/*
 * Define a constant-value field.
 */
void
CONSTANT_FIELD(const string &name, const pp_datatype *type,
		const pp_value &value)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	pp_constant_field_ptr field_ptr = new_pp_constant_field(type, value);
	current_context.add_dirent(name, field_ptr);
}
void
CONSTANT_FIELD(const string &name, const string &type, const pp_value &value)
{
	CONSTANT_FIELD(name, current_context.resolve_datatype(type), value);
}


/*
 * Define a pp_int datatype.
 */
pp_int *
INT(const string &name, const string &units)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "int: " + name);
	// sanity
	DASSERT_MSG(current_context.is_valid(), "invalid current_context");

	pp_int_ptr int_ptr = new_pp_int(units);
	if (name == "") {
		current_context.add_datatype(int_ptr);
	} else {
		current_context.add_datatype(name, int_ptr);
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
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "bitmask: " + name);
	// sanity
	DASSERT_MSG(current_context.is_valid(), "invalid current_context");

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
		current_context.add_datatype(bitmask_ptr);
	} else {
		current_context.add_datatype(name, bitmask_ptr);
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
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "enum: " + name);
	// sanity
	DASSERT_MSG(current_context.is_valid(), "invalid current_context");

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
		current_context.add_datatype(enum_ptr);
	} else {
		current_context.add_datatype(name, enum_ptr);
	}

	return enum_ptr.get();
}

pp_bool *
BOOL(const string &name, const string &true_str, const string &false_str)
{
	DASSERT_MSG(!current_context.is_readonly(),
		"current_context is read-only");
	DTRACE(TRACE_TYPES, "bool: " + name);
	// sanity
	DASSERT_MSG(current_context.is_valid(), "invalid current_context");

	pp_bool_ptr bool_ptr = new_pp_bool(true_str, false_str);
	if (name == "") {
		current_context.add_datatype(bool_ptr);
	} else {
		current_context.add_datatype(name, bool_ptr);
	}

	return bool_ptr.get();
}
