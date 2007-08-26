//FIXME: move this to devices/pci
#include "pp.h"
#include "pp_space.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pci_driver.h"
#include "pci_io.h"
#include "pci_binding.h"
#include "utils.h"

//All BARs look like this.
//macro BAR(name, offset) {
static void
base_address_register(const string &name, pp_scope *scope,
    const pp_binding *binding, pp_regaddr address)
{
	pp_direct_field_ptr field_ptr;
	pp_register_ptr reg_ptr;
	pp_register_ptr lower;
	pp_enum_ptr anon_enum;

	//scope name {
	pp_scope_ptr bar_scope = new_pp_scope();

	//reg32 lower(offset, preserve);
	lower = new_pp_register(binding, address, BITS32);
	bar_scope->add_register("%lower", lower);

	pp_direct_field_ptr width_field;
	pp_direct_field_ptr type_field;

	//field type(lower[0], enum() {
	//	MEM = 0
	//	IO = 1
	//});
	anon_enum = new_pp_enum();
	anon_enum->add_value("mem", 0);
	anon_enum->add_value("io", 1);
	type_field = new_pp_direct_field(anon_enum);
	type_field->add_regbits(lower.get(), 0, PP_MASK(1), 0);
	bar_scope->add_field("type", type_field);

	//if (type == MEM) {
	if (type_field->compare("mem")) {
		//field width(lower[2:1], enum() {
		//	BITS32 = 0,
		//	BITS20 = 1,
		//	BITS64 = 2,
		//});
		anon_enum = new_pp_enum();
		anon_enum->add_value("bits32", 0);
		anon_enum->add_value("bits20", 1);
		anon_enum->add_value("bits64", 2);
		width_field = new_pp_direct_field(anon_enum);
		width_field->add_regbits(lower.get(), 1, PP_MASK(2), 0);
		bar_scope->add_field("width", width_field);

		//field prefetch(lower[3], yes_no);
		field_ptr = new_pp_direct_field(
		    scope->resolve_datatype("yesno_t"));
		field_ptr->add_regbits(lower.get(), 3, PP_MASK(1), 0);
		bar_scope->add_field("prefetch", field_ptr);
	}

	//if (type == IO) {
	if (type_field->compare("io")) {
		//field addr((lower[15:2],zero[1:0]), ADDR16);
		field_ptr = new_pp_direct_field(
		    scope->resolve_datatype("addr16_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(2), 0);
		field_ptr->add_regbits(lower.get(), 2, PP_MASK(14), 2);
		bar_scope->add_field("addr", field_ptr);
	} else if (width_field->compare("bits32")
	    || width_field->compare("bits20")) {
		//} else if (width == BITS32 || width == BITS20) {
		//	field addr((lower[31:4],zero[3:0]), ADDR32);
		field_ptr = new_pp_direct_field(
		    scope->resolve_datatype("addr32_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(4), 0);
		field_ptr->add_regbits(lower.get(), 4, PP_MASK(28), 4);
		bar_scope->add_field("addr", field_ptr);
	} else {
		//} else {
		//	reg32 upper(offset+4, preserve);
		reg_ptr = new_pp_register(binding, address+4, BITS32);
		bar_scope->add_register("%upper", reg_ptr);
		//	field addr((upper[31:0],lower[31:4],zero[3:0]),
		//		ADDR64);
		field_ptr = new_pp_direct_field(
		    scope->resolve_datatype("addr64_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(4), 0);
		field_ptr->add_regbits(lower.get(), 4, PP_MASK(28), 4);
		field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(32), 32);
		bar_scope->add_field("addr", field_ptr);
		//}
	}
	scope->add_scope(name, bar_scope);
}

void
slot_id_capability(pp_scope *scope, const pp_binding *binding, int address)
{
	pp_register_ptr reg_ptr;
	pp_direct_field_ptr field_ptr;

	reg_ptr = new_pp_register(binding, address+2, BITS8);
	scope->add_register("%slot", reg_ptr);

	field_ptr = new_pp_direct_field(scope->resolve_datatype("int_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(5), 0);
	scope->add_field("nslots", field_ptr);

	field_ptr = new_pp_direct_field(scope->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 5, PP_MASK(1), 0);
	scope->add_field("first", field_ptr);

	reg_ptr = new_pp_register(binding, address+3, BITS8);
	scope->add_register("%chassis", reg_ptr);

	field_ptr = new_pp_direct_field(scope->resolve_datatype("int_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(8), 0);
	scope->add_field("chassis", field_ptr);
}

void
msi_capability(pp_scope *scope, const pp_binding *binding, int address)
{
	pp_direct_field_ptr field_ptr;
	pp_register_ptr reg_ptr;
	pp_enum_ptr anon_enum;

	// Message Control
	reg_ptr = new_pp_register(binding, address + 2, BITS16);
	scope->add_register("%msg_control", reg_ptr);

	field_ptr = new_pp_direct_field(scope->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(1), 0);
	scope->add_field("msi_enable", field_ptr);

	anon_enum = new_pp_enum();
	anon_enum->add_value("1", 0);
	anon_enum->add_value("2", 1);
	anon_enum->add_value("4", 2);
	anon_enum->add_value("8", 3);
	anon_enum->add_value("16", 4);
	anon_enum->add_value("32", 5);
	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg_ptr.get(), 1, PP_MASK(3), 0);
	scope->add_field("multi_msg_cap", field_ptr);

	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg_ptr.get(), 4, PP_MASK(3), 0);
	scope->add_field("multi_msg_en", field_ptr);

	pp_direct_field_ptr cap64_ptr = new_pp_direct_field(
		scope->resolve_datatype("yesno_t"));
	cap64_ptr->add_regbits(reg_ptr.get(), 7, PP_MASK(1), 0);
	scope->add_field("cap64", cap64_ptr);

	// Message Address
	reg_ptr = new_pp_register(binding, address + 4, BITS32);
	scope->add_register("%msg_addr_lo", reg_ptr);

	// is this is a 64 bit MSI capability?
	if (cap64_ptr->compare("no")) {
		field_ptr = new_pp_direct_field(
				scope->resolve_datatype("addr32_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(2), 0);
		field_ptr->add_regbits(reg_ptr.get(), 2, PP_MASK(30), 2);

		// define the data register
		reg_ptr = new_pp_register(binding, address + 8, BITS16);
	} else {
		// add the low half
		field_ptr = new_pp_direct_field(
				scope->resolve_datatype("addr64_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(2), 0);
		field_ptr->add_regbits(reg_ptr.get(), 2, PP_MASK(30), 2);

		// add the high half
		reg_ptr = new_pp_register(binding, address + 8, BITS32);
		scope->add_register("%msg_addr_hi", reg_ptr);
		field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(32), 32);

		// define the data register
		reg_ptr = new_pp_register(binding, address + 12, BITS16);
	}
	scope->add_field("msg_addr", field_ptr);

	// Message Data
	scope->add_register("%msg_data",  reg_ptr);
	field_ptr = new_pp_direct_field(scope->resolve_datatype("hex16_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(16), 0);
}

void
ssid_capability(pp_scope *scope, const pp_binding *binding, int address)
{
	pp_register_ptr reg_ptr;
	pp_direct_field_ptr field_ptr;

	regfield("ssvid", scope, binding, address+4, BITS16,
	    scope->resolve_datatype("pci_vendor_t"));

	regfield("ssid", scope, binding, address+6, BITS16,
	    scope->resolve_datatype("hex16_t"));
}

void
explore_capabilities(pp_space *space)
{
	// Handle the PCI capabilities linked-list.
	const pp_field *field;
	pp_value value;
	pp_scope_ptr scope_ptr;
	const pp_binding *binding = space->binding();

	// an enum for the know capabilities
	//FIXME: more!
	pp_enum_ptr anon_enum = new_pp_enum();
	anon_enum->add_value("power_mgmt", 0x01);
	anon_enum->add_value("agp", 0x02);
	anon_enum->add_value("vpd", 0x03);
	anon_enum->add_value("slot_id", 0x04);
	anon_enum->add_value("msi", 0x05);
	anon_enum->add_value("hot_swap", 0x06);
	//FIXME: 0x07
	anon_enum->add_value("ht", 0x08);
	//FIXME: 0x09
	anon_enum->add_value("usb2_debug_port", 0x0a);
	//FIXME: 0x0b
	//FIXME: 0x0c
	anon_enum->add_value("ssid", 0x0d);
	//FIXME: 0x0e
	//FIXME: 0x0f
	anon_enum->add_value("pcie", 0x10);
	//FIXME: MSI-X?
	//FIXME: PCI-X
	anon_enum->set_default("unknown");

	field = get_field(space, pp_path("status/caps"));
	value = field->read();
	if (value) {
		regfield("capptr", space, binding, 0x34, BITS8,
		    space->resolve_datatype("hex8_t"));

		field = get_field(space, pp_path("capptr"));
		pp_value ptr = field->read();

		int i = 0;
		std::string temp_string;
		while (ptr != 0 && ptr != 0xff) {
			scope_ptr = new_pp_scope();
			scope_ptr->set_parent(space);

			//FIXME: add a literal field base=$ptr?
			regfield("id", scope_ptr.get(), binding, ptr,
			    BITS8, anon_enum);
			regfield("next", scope_ptr.get(), binding, ptr+1,
			    BITS8, space->resolve_datatype("hex8_t"));

			// Explore Capability
			field = get_field(scope_ptr.get(), pp_path("/id"));
			value = field->read();
			if (value == field->lookup("power_mgmt")) {
				// PCI spec
			} else if (value == field->lookup("agp")) {
				// PCI spec
			} else if (value == field->lookup("vpd")) {
				// PCI spec
			} else if (value == field->lookup("slot_id")) {
				// PCI-PCI bridge spec
				slot_id_capability(scope_ptr.get(),
						binding, ptr);
			} else if (value == field->lookup("msi")) {
				// PCI-E spec
				msi_capability(scope_ptr.get(), binding, ptr);
			} else if (value == field->lookup("hot_swap")) {
			} else if (value == field->lookup("ht")) {
				// nvidia specs!
				// has sub-types! 15h = msi_map
			} else if (value == field->lookup("usb2_debug_port")) {
				// EHCI spec
			} else if (value == field->lookup("ssid")) {
				// found this through other specs
				ssid_capability(scope_ptr.get(), binding, ptr);
			} else if (value == field->lookup("pcie")) {
				// PCI-E spec
			}

			temp_string = "capability[" + to_string(i) + "]";
			space->add_scope(temp_string, scope_ptr);
			temp_string = temp_string + "/next";
			field = get_field(space, pp_path(temp_string));
			ptr = field->read();
			i++;
		}
	}
	//FIXME: scan EHCI extended capabilities
	//FIXME: PCIE Extended caps! { 16 bits ID, 4 bits version, 12 bits next
}

// PCI BRIDGE MACRO
void create_pci_bridge(pp_space *space)
{
	pp_register_ptr reg_ptr;
	pp_direct_field_ptr field_ptr;
	pp_scope_ptr scope_ptr;
	pp_enum_ptr anon_enum;
	const pp_binding *binding = space->binding();

	regfield("pri_bus", space, binding, 0x18, BITS8,
	    space->resolve_datatype("int_t"));
	regfield("sec_bus", space, binding, 0x19, BITS8,
	    space->resolve_datatype("int_t"));
	regfield("sub_bus", space, binding, 0x1a, BITS8,
	    space->resolve_datatype("int_t"));
	pp_int_ptr anon_int = new_pp_int("clocks");
	regfield("sec_latency", space, binding, 0x1b, BITS8, anon_int);

	// Secondary Status Register
	scope_ptr = new_pp_scope();

	reg_ptr = new_pp_register(binding, 0x1e, BITS16);
	scope_ptr->add_register("%sec_status", reg_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 5, PP_MASK(1), 0);
	scope_ptr->add_field("mhz66", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 7, PP_MASK(1), 0);
	scope_ptr->add_field("fbb", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 8, PP_MASK(1), 0);
	scope_ptr->add_field("mdperr", field_ptr);

	anon_enum = new_pp_enum();
	anon_enum->add_value("fast", 0);
	anon_enum->add_value("medium", 1);
	anon_enum->add_value("slow", 2);
	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg_ptr.get(), 9, PP_MASK(2), 0);
	scope_ptr->add_field("devsel", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 11, PP_MASK(1), 0);
	scope_ptr->add_field("sig_tabort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 12, PP_MASK(1), 0);
	scope_ptr->add_field("rcv_tabort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 13, PP_MASK(1), 0);
	scope_ptr->add_field("rcv_mabort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 14, PP_MASK(1), 0);
	scope_ptr->add_field("rcv_serr", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 15, PP_MASK(1), 0);
	scope_ptr->add_field("perr", field_ptr);

	space->add_scope("sec_status", scope_ptr);

	// scope io_window {
	scope_ptr = new_pp_scope();

	pp_register_ptr base_lo = new_pp_register(binding, 0x1c, BITS8);
	scope_ptr->add_register("%base_lo", base_lo);

	pp_register_ptr limit_lo = new_pp_register(binding, 0x1d, BITS8);
	scope_ptr->add_register("%limit_lo", limit_lo);

	pp_register_ptr base_hi = new_pp_register(binding, 0x30, BITS16);
	scope_ptr->add_register("%base_hi", base_hi);

	pp_register_ptr limit_hi = new_pp_register(binding, 0x32, BITS16);
	scope_ptr->add_register("%limit_hi", limit_hi);

	anon_enum = new_pp_enum();
	anon_enum->add_value("bits16", 0);
	anon_enum->add_value("bits32", 1);
	pp_direct_field_ptr width = new_pp_direct_field(anon_enum);
	width->add_regbits(base_lo.get(), 0, PP_MASK(4), 0);
	scope_ptr->add_field("width", width);

	// IO base/limit pair
	pp_value value = width->read();
	if (value == width->lookup("bits16")) {
		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr16_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(12), 0);
		field_ptr->add_regbits(base_lo.get(), 4, PP_MASK(4), 12);
		scope_ptr->add_field("base", field_ptr);

		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr16_t"));
		field_ptr->add_regbits(magic_ones, 0, PP_MASK(12), 0);
		field_ptr->add_regbits(limit_lo.get(), 4, PP_MASK(4), 12);
		scope_ptr->add_field("limit", field_ptr);
	} else if (value == width->lookup("bits32")) {
		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr32_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(12), 0);
		field_ptr->add_regbits(base_lo.get(), 4, PP_MASK(4), 12);
		field_ptr->add_regbits(base_hi.get(), 0, PP_MASK(16), 16);
		scope_ptr->add_field("base", field_ptr);

		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr32_t"));
		field_ptr->add_regbits(magic_ones, 0, PP_MASK(12), 0);
		field_ptr->add_regbits(limit_lo.get(), 4, PP_MASK(4), 12);
		field_ptr->add_regbits(limit_hi.get(), 0, PP_MASK(16), 16);
		scope_ptr->add_field("limit", field_ptr);
	}
	space->add_scope("io_window", scope_ptr);

	// scope mem_window {
	scope_ptr = new_pp_scope();

	reg_ptr = new_pp_register(binding, 0x20, BITS16);
	scope_ptr->add_register("%base", reg_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("addr32_t"));
	field_ptr->add_regbits(magic_zeros, 0, PP_MASK(20), 0);
	field_ptr->add_regbits(reg_ptr.get(), 4, PP_MASK(12), 20);
	scope_ptr->add_field("base", field_ptr);

	reg_ptr = new_pp_register(binding, 0x22, BITS16);
	scope_ptr->add_register("%limit", reg_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("addr32_t"));
	field_ptr->add_regbits(magic_ones, 0, PP_MASK(20), 0);
	field_ptr->add_regbits(reg_ptr.get(), 4, PP_MASK(12), 20);
	scope_ptr->add_field("limit", field_ptr);

	space->add_scope("mem_window", scope_ptr);

	// scope prefetch_window {
	scope_ptr = new_pp_scope();

	base_lo = new_pp_register(binding, 0x24, BITS16);
	scope_ptr->add_register("%base_lo", base_lo);

	limit_lo = new_pp_register(binding, 0x26, BITS16);
	scope_ptr->add_register("%limit_lo", limit_lo);

	base_hi = new_pp_register(binding, 0x28, BITS32);
	scope_ptr->add_register("%base_hi", base_hi);

	limit_hi = new_pp_register(binding, 0x2c, BITS32);
	scope_ptr->add_register("%limit_hi", limit_hi);

	anon_enum = new_pp_enum();
	anon_enum->add_value("bits32", 0);
	anon_enum->add_value("bits64", 1);
	width = new_pp_direct_field(anon_enum);
	width->add_regbits(base_lo.get(), 0, PP_MASK(4), 0);
	scope_ptr->add_field("width", width);

	// prefetch base/limit pair
	value = width->read();
	if (value == width->lookup("bits32")) {
		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr32_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(20), 0);
		field_ptr->add_regbits(base_lo.get(), 4, PP_MASK(12), 20);
		scope_ptr->add_field("base", field_ptr);

		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr32_t"));
		field_ptr->add_regbits(magic_ones, 0, PP_MASK(20), 0);
		field_ptr->add_regbits(limit_lo.get(), 4, PP_MASK(12), 20);
		scope_ptr->add_field("limit", field_ptr);
	} else if (value == width->lookup("bits64")) {
		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr64_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(20), 0);
		field_ptr->add_regbits(base_lo.get(), 4, PP_MASK(12), 20);
		field_ptr->add_regbits(base_hi.get(), 0, PP_MASK(32), 32);
		scope_ptr->add_field("base", field_ptr);

		field_ptr = new_pp_direct_field(
				space->resolve_datatype("addr64_t"));
		field_ptr->add_regbits(magic_ones, 0, PP_MASK(20), 0);
		field_ptr->add_regbits(limit_lo.get(), 4, PP_MASK(12), 20);
		field_ptr->add_regbits(limit_hi.get(), 0, PP_MASK(32), 32);
		scope_ptr->add_field("limit", field_ptr);
	}
	space->add_scope("prefetch_window", scope_ptr);

	// Capabilities
	explore_capabilities(space);

	// Expansion ROM Base Address

	reg_ptr = new_pp_register(binding, 0x38, BITS32);
	space->add_register("%rombase", reg_ptr);
	scope_ptr = new_pp_scope();
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(1), 0);
	scope_ptr->add_field("en", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("hex32_t"));
	field_ptr->add_regbits(magic_zeros, 0, PP_MASK(11), 0);
	field_ptr->add_regbits(reg_ptr.get(), 11, PP_MASK(21), 11);
	scope_ptr->add_field("addr", field_ptr);
	space->add_scope("rombase", scope_ptr);

	// Bridge Control
	reg_ptr = new_pp_register(binding, 0x3e, BITS16);
	space->add_register("%bridge_ctrl", reg_ptr);

	scope_ptr = new_pp_scope();

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(1), 0);
	scope_ptr->add_field("perr", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 1, PP_MASK(1), 0);
	scope_ptr->add_field("serr", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 2, PP_MASK(1), 0);
	scope_ptr->add_field("isa", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 3, PP_MASK(1), 0);
	scope_ptr->add_field("vga", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 4, PP_MASK(1), 0);
	scope_ptr->add_field("vga16", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 5, PP_MASK(1), 0);
	scope_ptr->add_field("mabort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 6, PP_MASK(1), 0);
	scope_ptr->add_field("sec_reset", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 7, PP_MASK(1), 0);
	scope_ptr->add_field("fbb", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 8, PP_MASK(1), 0);
	scope_ptr->add_field("pri_discard", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 9, PP_MASK(1), 0);
	scope_ptr->add_field("sec_discard", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 10, PP_MASK(1), 0);
	scope_ptr->add_field("discard_status", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 11, PP_MASK(1), 0);
	scope_ptr->add_field("discard_serr", field_ptr);

	space->add_scope("bridge_ctrl", scope_ptr);
}

// DEVICE MACRO
static void
create_device(pp_space *space)
{
	pp_direct_field_ptr field_ptr;
	pp_register_ptr reg_ptr;
	const pp_binding *binding = space->binding();
	pp_scope_ptr scope_ptr;

	//   if bar1 is not defined, bar0 must be 64 bit, bar2 is ok.
	//   if bar1.type is not 64 bit memory, bar2 is ok.
	//if (!defined(bar1) ||
	//    !(bar1.type == MEM && bar1.width == BITS64)) {
	//	BAR(bar2, 0x18)
	if (!dirent_defined(space, pp_path("bar1"))) {
		base_address_register("bar2", space, binding, 0x18);
	} else {
		if (!(get_field(space, pp_path("bar1/type"))->compare("mem")
		   && get_field(space, pp_path("bar1/width"))->compare("bits64"))) {
			base_address_register("bar2", space, binding, 0x18);
		}
	}

	//if (!defined(bar2) ||
	//    !(bar2.type == MEM && bar2.width == BITS64)) {
	//	BAR(bar3, 0x1c)
	if (!dirent_defined(space, pp_path("bar2"))) {
		base_address_register("bar3", space, binding, 0x1c);
	} else {
		if (!(get_field(space, pp_path("bar2/type"))->compare("mem")
		   && get_field(space, pp_path("bar2/width"))->compare("bits64"))) {
			base_address_register("bar3", space, binding, 0x1c);
		}
	}

	//if (!defined(bar3) ||
	//    !(bar3.type == MEM && bar3.width == BITS64)) {
	//	BAR(bar4, 0x20)
	if (!dirent_defined(space, pp_path("bar3"))) {
		base_address_register("bar4", space, binding, 0x20);
	} else {
		if (!(get_field(space, pp_path("bar3/type"))->compare("mem")
		   && get_field(space, pp_path("bar3/width"))->compare("bits64"))) {
			base_address_register("bar4", space, binding, 0x20);
		}
	}

	//if (!defined(bar4) ||
	//    !(bar4.type == MEM && bar4.width == BITS64)) {
	//	BAR(bar5, 0x24)
	if (!dirent_defined(space, pp_path("bar4"))) {
		base_address_register("bar5", space, binding, 0x24);
	} else {
		if (!(get_field(space, pp_path("bar4/type"))->compare("mem")
		   && get_field(space, pp_path("bar4/width"))->compare("bits64"))) {
			base_address_register("bar5", space, binding, 0x24);
		}
	}

	//regfield32 cisptr(0x28, ADDR32);
	regfield("cisptr", space, binding, 0x28, BITS32,
	    space->resolve_datatype("addr32_t"));

	//regfield16 subvendor(0x2c, PCI_VENDOR);
	regfield("subvendor", space, binding, 0x2c, BITS16,
	    space->resolve_datatype("pci_vendor_t"));

	//regfield16 subdevice(0x2e, HEX16);
	regfield("subdevice", space, binding, 0x2e, BITS16,
	    space->resolve_datatype("hex16_t"));

	// Capabilities
	explore_capabilities(space);

	//reg32 rombase(0x30, preserve);
	reg_ptr = new_pp_register(binding, 0x30, BITS32);
	space->add_register("%rombase", reg_ptr);
	//scope rombase {
	scope_ptr = new_pp_scope();
	//	field en(rombase[0], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(1), 0);
	scope_ptr->add_field("en", field_ptr);

	//	field addr({rombase[31:11],zero[10:0]}, HEX32);
	field_ptr = new_pp_direct_field(space->resolve_datatype("hex32_t"));
	field_ptr->add_regbits(magic_zeros, 0, PP_MASK(11), 0);
	field_ptr->add_regbits(reg_ptr.get(), 11, PP_MASK(21), 11);
	scope_ptr->add_field("addr", field_ptr);
	//}
	space->add_scope("rombase", scope_ptr);

	//regfield8 mingnt(0x3e, INT("1/4 usec"));
	regfield("mingnt", space, binding, 0x3e, BITS8,
	    space->resolve_datatype("int_t"));
	//regfield8 maxlat(0x3f, INT("1/4 usec"));
	regfield("maxlat", space, binding, 0x3f, BITS8,
	    space->resolve_datatype("int_t"));
}

pp_space_ptr
pci_generic_space(pp_const_binding_ptr binding_ptr, const pp_platform *platform)
{
	const pp_binding *binding = binding_ptr.get();
	pp_space_ptr space_ptr = new_pp_space(binding_ptr);
	pp_space *space = space_ptr.get();
	pp_register_ptr reg_ptr;
	const pp_register *reg;
	pp_direct_field_ptr field_ptr;
	const pp_field *field;
	pp_enum_ptr anon_enum;
	pp_int_ptr anon_int;
	pp_bitmask_ptr anon_bitmask;
	pp_scope_ptr scope;

	/* this allows easier type resolution */
	space->set_parent(platform);

	// regrange32 PCI(0, 4096, 4, PRESERVE);
	for (int i = 0; i < 4096; i += 4) {
		reg_ptr = new_pp_register(binding, i, BITS32);
		space->add_register(to_string(
		    boost::format("%%PCI[%04x]") %i), reg_ptr);
	}

	// regfield16 vendor(0x00, PCI_VENDOR);
	regfield("vendor", space, binding, 0x00, BITS16,
	    space->resolve_datatype("pci_vendor_t"));

	// regfield16 device(0x02, HEX16);
	regfield("device", space, binding, 0x02, BITS16,
	    space->resolve_datatype("hex16_t"));

	// reg16 command(0x04, preserve);
	reg_ptr = new_pp_register(binding, 0x04, BITS16);
	reg = reg_ptr.get();
	space->add_register("%command", reg_ptr);

	// scope command {
	scope = new_pp_scope();
	//	field io(command[0], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 0, PP_MASK(1), 0);
	scope->add_field("io", field_ptr);
	//	field mem(command[1], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 1, PP_MASK(1), 0);
	scope->add_field("mem", field_ptr);
	//	field bm(command[2], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 2, PP_MASK(1), 0);
	scope->add_field("bm", field_ptr);
	//	field special(command[3], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 3, PP_MASK(1), 0);
	scope->add_field("special", field_ptr);
	//	field mwinv(command[4], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 4, PP_MASK(1), 0);
	scope->add_field("mwinv", field_ptr);
	//	field vgasnoop(command[5], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 5, PP_MASK(1), 0);
	scope->add_field("vgasnoop", field_ptr);
	//	field perr(command[6], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 6, PP_MASK(1), 0);
	scope->add_field("perr", field_ptr);
	//	field step(command[7], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 7, PP_MASK(1), 0);
	scope->add_field("step", field_ptr);
	//	field serr(command[8], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 8, PP_MASK(1), 0);
	scope->add_field("serr", field_ptr);
	//	field fbb(command[9], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 9, PP_MASK(1), 0);
	scope->add_field("fbb", field_ptr);
	//	field intr(command[10], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 10, PP_MASK(1), 0);
	scope->add_field("intr", field_ptr);
	//}
	space->add_scope("command", scope);

	// reg16 status(0x06, preserve);
	reg_ptr = new_pp_register(binding, 0x06, BITS16);
	reg = reg_ptr.get();
	space->add_register("%status", reg_ptr);

	// scope status {
	scope = new_pp_scope();
	//	field intr(status[3], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 3, PP_MASK(1), 0);
	scope->add_field("intr", field_ptr);
	//	field caps(status[4], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 4, PP_MASK(1), 0);
	scope->add_field("caps", field_ptr);
	//	field cap66(status[5], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 5, PP_MASK(1), 0);
	scope->add_field("mhz66", field_ptr);
	//	field fbb(status[7], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 7, PP_MASK(1), 0);
	scope->add_field("fbb", field_ptr);
	//	field mdperr(status[8], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 8, PP_MASK(1), 0);
	scope->add_field("mdperr", field_ptr);
	//	field devsel(status[10:9], enum() {
	//		FAST = 0
	//		MEDIUM = 1
	//		SLOW = 2
	//	});
	anon_enum = new_pp_enum();
	anon_enum->add_value("fast", 0);
	anon_enum->add_value("medium", 1);
	anon_enum->add_value("slow", 2);
	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg, 9, PP_MASK(2), 0);
	scope->add_field("devsel", field_ptr);
	//	field sigtabort(status[11], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 11, PP_MASK(1), 0);
	scope->add_field("sigtabort", field_ptr);
	//	field rcvtabort(status[12], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 12, PP_MASK(1), 0);
	scope->add_field("rcvtabort", field_ptr);
	//	field rcvmabort(status[13], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 13, PP_MASK(1), 0);
	scope->add_field("rcvmabort", field_ptr);
	//	field serr(status[14], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 14, PP_MASK(1), 0);
	scope->add_field("serr", field_ptr);
	//	field perr(status[15], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 15, PP_MASK(1), 0);
	scope->add_field("perr", field_ptr);
	//}
	space->add_scope("status", scope);

	//regfield8 class(0x0b, PCI_CLASS);
	regfield("class", space, binding, 0x0b, BITS8,
	    space->resolve_datatype("pci_class_t"));

	//regfield8 subclass(0x0a, ...); // per-class subclasses
	pp_enum_ptr subclass_type = new_pp_enum();
	field = get_field(space, pp_path("class"));
	pp_value classcode = field->read();
	if (classcode == field->lookup("pre-classcode device")) {
		subclass_type->add_value("non-VGA device", 0x00);
		subclass_type->add_value("VGA device", 0x01);
	} else if (classcode == field->lookup("mass storage controller")) {
		subclass_type->add_value("SCSI controller", 0x00);
		subclass_type->add_value("IDE controller", 0x01);
		subclass_type->add_value("floppy controller", 0x02);
		subclass_type->add_value("IPI bus controller", 0x03);
		subclass_type->add_value("RAID controller", 0x04);
		subclass_type->add_value("ATA controller with ADMA", 0x05);
		subclass_type->add_value("SATA controller", 0x06);
		subclass_type->add_value("SAS controller", 0x06);
		subclass_type->add_value("other", 0x80);
	} else if (classcode == field->lookup("network controller")) {
		subclass_type->add_value("ethernet controller", 0x00);
		subclass_type->add_value("token Ring controller", 0x01);
		subclass_type->add_value("FDDI controller", 0x02);
		subclass_type->add_value("ATM controller", 0x03);
		subclass_type->add_value("ISDN controller", 0x04);
		subclass_type->add_value("other", 0x80);
	} else if (classcode == field->lookup("display controller")) {
		subclass_type->add_value("VGA controller", 0x00);
		subclass_type->add_value("XGA controller", 0x01);
		subclass_type->add_value("3D controller", 0x02);
		subclass_type->add_value("other", 0x80);
	} else if (classcode == field->lookup("multimedia device")) {
		subclass_type->add_value("video device", 0x00);
		subclass_type->add_value("audio device", 0x01);
		subclass_type->add_value("computer telephony device", 0x02);
		subclass_type->add_value("other", 0x80);
	} else if (classcode == field->lookup("memory controller")) {
		subclass_type->add_value("RAM", 0x00);
		subclass_type->add_value("flash", 0x01);
		subclass_type->add_value("other", 0x80);
	} else if (classcode == field->lookup("bridge device")) {
		subclass_type->add_value("host bridge", 0x00);
		subclass_type->add_value("ISA bridge", 0x01);
		subclass_type->add_value("EISA bridge", 0x02);
		subclass_type->add_value("MCA bridge", 0x03);
		subclass_type->add_value("PCI bridge", 0x04);
		subclass_type->add_value("PCMCIA bridge", 0x05);
		subclass_type->add_value("NuBus bridge", 0x06);
		subclass_type->add_value("CardBus bridge", 0x07);
		subclass_type->add_value("RACEway bridge", 0x08);
		subclass_type->add_value("other", 0x80);
	} else {
		//TODO: classes 0x07-0x11
	}
	regfield("subclass", space, binding, 0x0a, BITS8, subclass_type);

	//FIXME: would be better to have detailed progintfs
	//regfield8 progintf(0x09, HEX8);
	reg_ptr = new_pp_register(binding, 0x09, BITS8);
	reg = reg_ptr.get();
	space->add_register("%progintf", reg_ptr);
	field_ptr = new_pp_direct_field(space->resolve_datatype("hex8_t"));
	field_ptr->add_regbits(reg, 0, PP_MASK(BITS8), 0);
	space->add_field("progintf", field_ptr);

	//regfield8 revision(0x08, HEX8);
	regfield("revision", space, binding, 0x08, BITS8,
	    space->resolve_datatype("hex8_t"));

	//regfield8 cacheline(0x0c, int("dwords"));
	anon_int = new_pp_int("DWORDs");
	regfield("cacheline", space, binding, 0x0c, BITS8, anon_int);

	//regfield8 latency(0x0d, int("clocks"));
	anon_int = new_pp_int("clocks");
	regfield("latency", space, binding, 0x0d, BITS8, anon_int);

	//reg8 hdrtype(0x0e, preserve);
	reg_ptr = new_pp_register(binding, 0x0e, BITS8);
	reg = reg_ptr.get();
	space->add_register("%hdrtype", reg_ptr);

	//scope hdrtype {
	scope = new_pp_scope();
	//	field type(hdrtype[6:0], enum() {
	//		DEVICE = 0;
	//		PCI_BRIDGE = 1;
	//		CARD_BRIDGE = 2;
	//		//TODO: incomplete list?
	//	    });
	anon_enum = new_pp_enum();
	anon_enum->add_value("device", 0);
	anon_enum->add_value("PCI bridge", 1);
	anon_enum->add_value("CardBus bridge", 2);
	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg, 0, PP_MASK(7), 0);
	scope->add_field("type", field_ptr);
	//	field multi_function(hdrtype[7], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 7, PP_MASK(1), 0);
	scope->add_field("multi_function", field_ptr);
	//}
	space->add_scope("hdrtype", scope);

	//reg8 bist(0x0f, preserve);
	reg_ptr = new_pp_register(binding, 0x0f, BITS8);
	reg = reg_ptr.get();
	space->add_register("%bist", reg_ptr);

	//scope bist {
	scope = new_pp_scope();
	//	field capable(bist[7], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 7, PP_MASK(1), 0);
	scope->add_field("bist_capable", field_ptr);
	//	field start(bist[6], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg, 6, PP_MASK(1), 0);
	scope->add_field("start_bist", field_ptr);
	//	field code(bist[3:0], hex8);
	field_ptr = new_pp_direct_field(space->resolve_datatype("hex8_t"));
	field_ptr->add_regbits(reg, 0, PP_MASK(4), 0);
	scope->add_field("completion_code", field_ptr);
	//}

	//regfield8 intline(0x3c, int);
	regfield("intline", space, binding, 0x3c, BITS8,
	    space->resolve_datatype("int_t"));

	//regfield8 intpin(0x3c, enum() {
	//	NONE = 0
	//	INTA = 1
	//	INTB = 2
	//	INTC = 3
	//	INTD = 4
	//   });
	anon_enum = new_pp_enum();
	anon_enum->add_value("NONE", 0);
	anon_enum->add_value("INTA", 1);
	anon_enum->add_value("INTB", 2);
	anon_enum->add_value("INTC", 3);
	anon_enum->add_value("INTD", 4);
	regfield("intpin", space, binding, 0x3d, BITS8, anon_enum);

	// Base Address Registers - all devices have at least 2 BARs.
	//BAR(bar0, 0x10) {
	base_address_register("bar0", space, binding, 0x10);

	//if (!(bar0.type == MEM && bar0.width == BITS64)) {
	//	BAR(bar1, 0x14)
	//}
	if (!(get_field(space, pp_path("bar0/type"))->compare("mem")
	   && get_field(space, pp_path("bar0/width"))->compare("bits64"))) {
		base_address_register("bar1", space, binding, 0x14);
	}

	// if (hdrtype.type == DEVICE) {
	field = get_field(space, pp_path("hdrtype/type"));
	if (field->compare("device")) {
		create_device(space);
	} else if (field->compare("PCI bridge")) {
		create_pci_bridge(space);
	} else if (field->compare("CardBus bridge")) {
		//FIXME:
	}

	return space_ptr;
}
