#include <iostream>

#include "pp.h"
#include "pp_space.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pci_driver.h"
#include "pci_io.h"
#include "pci_binding.h"
#include "utils.h"

void
init_crap(pp_platform_ptr platform)
{
	pp_enum_ptr e;

	e = new_pp_enum();
	e->add_value("Intel", 0x8086);
	e->add_value("AMD", 0x1022);
	e->add_value("NVidia", 0x10de);
	e->add_value("Broadcom", 0x14e4);
	e->add_value("Silicon Image", 0x1095);
	e->set_default("Unknown");
	platform->add_datatype("pci_vendor_t", e);

	platform->add_datatype("int_t", new_pp_int());

	platform->add_datatype("hex8_t", new_pp_hex(BITS8));
	platform->add_datatype("hex16_t", new_pp_hex(BITS16));
	platform->add_datatype("hex32_t", new_pp_hex(BITS32));
	platform->add_datatype("addr16_t", new_pp_hex(BITS16));
	platform->add_datatype("addr32_t", new_pp_hex(BITS32));
	platform->add_datatype("addr64_t", new_pp_hex(BITS64));

	e = new_pp_enum();
	e->add_value("yes", 1);
	e->add_value("no", 0);
	platform->add_datatype("yesno_t", e);

	e = new_pp_enum();
	e->add_value("pre-classcode device", 0x00);
	e->add_value("mass storage controller", 0x01);
	e->add_value("network controller", 0x02);
	e->add_value("display controller", 0x03);
	e->add_value("multimedia device", 0x04);
	e->add_value("memory controller", 0x05);
	e->add_value("bridge device", 0x06);
	e->add_value("simple communication controller", 0x07);
	e->add_value("base system peripheral", 0x08);
	e->add_value("input device", 0x09);
	e->add_value("docking station", 0x0a);
	e->add_value("processor", 0x0b);
	e->add_value("serial bus controller", 0x0c);
	e->add_value("wireless controller", 0x0d);
	e->add_value("intelligent I/O controller", 0x0e);
	e->add_value("satellite communication controller", 0x0f);
	e->add_value("encryption/decryption controller", 0x10);
	e->add_value("data acquisition / signal processing controller", 0x11);
	//TODO: incomplete list
	e->set_default("unknown class");
	platform->add_datatype("pci_class_t", e);
}

//All BARs look like this.
//macro BAR(name, offset) {
void
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
	if (type_field->read() == type_field->lookup("mem")) {
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

	if (type_field->read() == type_field->lookup("io")) {
		//if (type == IO) {
		//	field addr((lower[15:2],zero[1:0]), ADDR16);
		field_ptr = new_pp_direct_field(
		    scope->resolve_datatype("addr16_t"));
		field_ptr->add_regbits(magic_zeros, 0, PP_MASK(2), 0);
		field_ptr->add_regbits(lower.get(), 2, PP_MASK(14), 2);
		bar_scope->add_field("addr", field_ptr);
	} else if (width_field->read() == width_field->lookup("bits32") ||
		 width_field->read() == width_field->lookup("bits20")) {
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

void message_capability_structure (pp_space *space, pp_scope *scope,
	const pp_binding *binding, int address)
{
	pp_direct_field_ptr field_ptr;
	pp_register_ptr reg_ptr;
	pp_enum_ptr anon_enum;
	pp_value value;

	// Message Control
	reg_ptr = new_pp_register(binding, address + 16, BITS16);
	scope->add_register("%msg_control", reg_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(1), 0);
	scope->add_field("msi_enable", field_ptr);

	anon_enum = new_pp_enum();
	anon_enum->add_value("one", 0);
	anon_enum->add_value("two", 1);
	anon_enum->add_value("four", 2);
	anon_enum->add_value("eight", 3);
	anon_enum->add_value("sixteen", 4);
	anon_enum->add_value("thirtytwo", 5);
	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg_ptr.get(), 1, PP_MASK(3), 1);
	scope->add_field("mmsg_capable", field_ptr);

	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg_ptr.get(), 4, PP_MASK(3), 4);
	scope->add_field("mmsg_enable", field_ptr);

	pp_direct_field_ptr cap64_ptr = new_pp_direct_field(
		space->resolve_datatype("yesno_t"));
	cap64_ptr->add_regbits(reg_ptr.get(), 7, PP_MASK(1), 7);
	scope->add_field("cap64", cap64_ptr);

	// Message Address
	reg_ptr = new_pp_register(binding, address + 32, BITS32);
	scope->add_register("%msg_addr", reg_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("addr32_t"));
	field_ptr->add_regbits(reg_ptr.get(), 2, PP_MASK(30), 2);
	scope->add_field("msg_addr", field_ptr);

	// Message Upper Address
	value = cap64_ptr->read();
	if (value == cap64_ptr->lookup("yes")) {
		regfield("msg_up_addr", scope, binding, address + 64, BITS32,
		    space->resolve_datatype("addr32_t"));
	}

	// Message Data
	regfield("msg_data", scope, binding, address + 80, BITS16,
		    space->resolve_datatype("addr16_t"));
}

void explore_capabilities(pp_space *space)
{
	// Handle the PCI capabilities linked-list.
	//if (status.caps) {
	const pp_field *field = get_field(space, pp_path("status/caps"));
	pp_value value = field->read();
	pp_scope_ptr scope_ptr;
	pp_enum_ptr anon_enum = new_pp_enum();
	anon_enum->add_value("power_mgmt", 1);
	anon_enum->add_value("agp", 2);
	anon_enum->add_value("vpd", 3);
	anon_enum->add_value("slot_id", 4);
	anon_enum->add_value("msi", 5);
	anon_enum->add_value("hot_swap", 6);
	const pp_binding * binding = space->binding ();
	if (value) {
	//	regfield8 capptr(0x34, HEX8);
		regfield("capptr", space, binding, 0x34, BITS8,
		    space->resolve_datatype("hex8_t"));

	//	var $ptr = capptr;
		field = get_field(space, pp_path("capptr"));
		pp_value ptr = field->read();

	//	var $i = 0;
		int i = 0;
		std::string temp_string;
	//	while ($ptr != 0) {
		while (ptr != 0 && ptr != 0xff) {
	//		scope capability[$i] {
			scope_ptr = new_pp_scope();
	//			//FIXME: add a lityeral field base=$ptr
	//			regfield8 id($ptr, HEX8);
			regfield("id", scope_ptr.get(), binding, ptr,
			    BITS8, anon_enum);
	//			regfield8 next($ptr+1, HEX8);
			regfield("next", scope_ptr.get(), binding, ptr+1,
			    BITS8, space->resolve_datatype("hex8_t"));
	//		//FIXME:  rest of caps fields
	//		}
			temp_string = "capability[" + to_string(i) + "]";

			// Explore Capability
			field = get_field(scope_ptr.get(), pp_path("/id"));
			value = field->read();
			if (value == field->lookup("power_mgmt")) {
			} else if (value == field->lookup("agp")) {
			} else if (value == field->lookup("vpd")) {
			} else if (value == field->lookup("slot_id")) {
			} else if (value == field->lookup("msi")) {
				message_capability_structure (space,
					scope_ptr.get(), binding, ptr);
			} else if (value == field->lookup("hot_swap")) {
			}

			space->add_scope(temp_string, scope_ptr);
	//		$ptr = capability[$i].next;
			temp_string = temp_string + "/next";
			field = get_field(space, pp_path(temp_string));
			ptr = field->read();
	//		$i = $i+1;
			i++;
	//	}
		}
	//}
	}
}

// PCI BRIDGE MACRO
//FIXME: sync same as create_device
void create_pci_bridge (pp_space *space)
{
	pp_register_ptr reg_ptr;
	pp_direct_field_ptr field_ptr;
	const pp_binding *binding = space->binding();

	//	regfield8 pri_bus(0x18, INT());
	regfield("pri_bus", space, binding, 0x18, BITS8,
		space->resolve_datatype("int_t"));
	//	regfield8 sec_bus(0x19, INT());
	regfield("sec_bus", space, binding, 0x19, BITS8,
		space->resolve_datatype("int_t"));
	//	regfield8 sub_bus(0x1a, INT());
	regfield("sub_bus", space, binding, 0x1a, BITS8,
		space->resolve_datatype("int_t"));
	//	regfield8 sec_latency(0x1b, INT("clocks"));
	regfield("sec_latency", space, binding, 0x1b, BITS8,
		space->resolve_datatype("int_t"));

	//	scope io_window {
	pp_scope_ptr scope = new_pp_scope();
	//		reg8 base_lo(1c, preserve);
		pp_register_ptr base_lo = new_pp_register(binding, 0x1c, BITS8);
		scope->add_register("%base_lo", base_lo);

	//		reg8 limit_lo(0x1d, preserve);
		pp_register_ptr limit_lo = new_pp_register(binding, 0x1d, BITS8);
		scope->add_register("%limit_lo", limit_lo);

	//		reg16 base_hi(0x30, preserve);
		pp_register_ptr base_hi = new_pp_register(binding, 0x30, BITS8);
		scope->add_register("%base_hi", base_hi);

	//		reg16 limit_hi(0x32, preserve);
		pp_register_ptr limit_hi = new_pp_register(binding, 0x32, BITS8);
		scope->add_register("%limit_hi", limit_hi);

	//		field width(base_lo[3:0], enum() {
	//			BITS16 = 0;
	//			BITS32 = 1;
	//		    });
		pp_enum_ptr anon_enum = new_pp_enum();
		anon_enum->add_value("bits16", 0);
		anon_enum->add_value("bits32", 1);
		pp_direct_field_ptr width = new_pp_direct_field(anon_enum);
		width->add_regbits(base_lo.get(), 0, PP_MASK(4), 0);
		scope->add_field("width", width);
		pp_value value = width->read();

	//		if (width == BITS16) {
		if (value == width->lookup("bits16"))
		{
	//			field base((base_lo[7:4],zero[11:0]},
	//				ADDR16));
			field_ptr = new_pp_direct_field(space->resolve_datatype("addr16_t"));
			field_ptr->add_regbits(base_lo.get(), 4, PP_MASK(4), 11);
			field_ptr->add_regbits(magic_zeros, 0, PP_MASK(12), 0);
			scope->add_field("base", field_ptr);
			
	//			field limit((limit_lo[7:4],one[11:0]},
	//				ADDR16));
			field_ptr = new_pp_direct_field(space->resolve_datatype("addr16_t"));
			field_ptr->add_regbits(limit_lo.get(), 4, PP_MASK(4), 11);
			field_ptr->add_regbits(magic_ones, 0, PP_MASK(12), 0);
			scope->add_field("limit", field_ptr);
		}
	//		} else if (width == BITS32) {
		else if (value == width->lookup("bits32"))
		{
	//			field base(
	//				(base_hi[],base_lo[7:4],zero[11:0]),
	//				ADDR32);
			field_ptr = new_pp_direct_field(space->resolve_datatype("addr32_t"));
			field_ptr->add_regbits(base_hi.get(), 0, PP_MASK(32), 16);
			field_ptr->add_regbits(base_lo.get(), 4, PP_MASK(4), 11);
			field_ptr->add_regbits(magic_zeros, 0, PP_MASK(12), 0);
			scope->add_field("base", field_ptr);

	//			field limit(
	//				(limit_hi[],limit_lo[15:12],one[11:0]),
	//				ADDR32);
			field_ptr = new_pp_direct_field(space->resolve_datatype("addr32_t"));
			field_ptr->add_regbits(limit_hi.get(), 0, PP_MASK(32), 16);
			field_ptr->add_regbits(limit_lo.get(), 12, PP_MASK(4), 11);
			field_ptr->add_regbits(magic_ones, 0, PP_MASK(12), 0);
			scope->add_field("limit", field_ptr);
	//		}
		}
	space->add_scope("io_window", scope);

	// Secondary Status Register
	scope = new_pp_scope();

	reg_ptr = new_pp_register(binding, 0x1e, BITS16);
	scope->add_register("%sec_status", reg_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 5, PP_MASK(1), 5);
	scope->add_field("cap66", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 7, PP_MASK(1), 7);
	scope->add_field("fbb", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 8, PP_MASK(1), 8);
	scope->add_field("master_data_perr", field_ptr);

	anon_enum = new_pp_enum();
	anon_enum->add_value("fast", 0);
	anon_enum->add_value("medium", 1);
	anon_enum->add_value("slow", 2);
	field_ptr = new_pp_direct_field(anon_enum);
	field_ptr->add_regbits(reg_ptr.get(), 9, PP_MASK(2), 9);
	scope->add_field("timing", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 11, PP_MASK(1), 11);
	scope->add_field("sig_tabort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 12, PP_MASK(1), 12);
	scope->add_field("rcv_tabort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 13, PP_MASK(1), 13);
	scope->add_field("rcv_mabort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 14, PP_MASK(1), 14);
	scope->add_field("rcv_serr", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 15, PP_MASK(1), 15);
	scope->add_field("perr", field_ptr);

	space->add_scope("sec_status", scope);

	// Memory Base
	reg_ptr = new_pp_register(binding, 0x20, BITS16);
	space->add_register("%mem_base", reg_ptr);

	scope = new_pp_scope();

	field_ptr = new_pp_direct_field(space->resolve_datatype("hex8_t"));
	field_ptr->add_regbits(reg_ptr.get(), 5, PP_MASK(12), 5);
	scope->add_field("mem_base_lo", field_ptr);

	// Memory Limit
	reg_ptr = new_pp_register(binding, 0x22, BITS16);
	space->add_register("%mem_limit", reg_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("hex8_t"));
	field_ptr->add_regbits(reg_ptr.get(), 5, PP_MASK(12), 5);
	scope->add_field("mem_limit_lo", field_ptr);

	space->add_scope("mem_window", scope);

	// Prefetchable Memory Base Upper
	scope = new_pp_scope();
	regfield("pre_mem_base", scope.get(), binding, 0x28, BITS32,
		space->resolve_datatype("addr32_t"));

	// Prefetchable Memory Limit Upper
	regfield("pre_mem_limit", scope.get(), binding, 0x2c, BITS32,
		space->resolve_datatype("addr32_t"));
	space->add_scope("pre_mem_window", scope);

	// CAPABILITIES
	explore_capabilities(space);

	// Expansion ROM Base Address
	
	//reg32 rombase(0x38, preserve);
	reg_ptr = new_pp_register(binding, 0x38, BITS32);
	space->add_register("%rombase", reg_ptr);
	//scope rombase {
	scope = new_pp_scope();
	//	field en(rombase[0], yes_no);
	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(1), 0);
	scope->add_field("en", field_ptr);

	//	field addr({rombase[31:11],zero[10:0]}, HEX32);
	field_ptr = new_pp_direct_field(space->resolve_datatype("hex32_t"));
	field_ptr->add_regbits(magic_zeros, 0, PP_MASK(11), 0);
	field_ptr->add_regbits(reg_ptr.get(), 11, PP_MASK(21), 11);
	scope->add_field("addr", field_ptr);
	//}
	space->add_scope("rombase", scope);

	// Bridge Control
	reg_ptr = new_pp_register(binding, 0x3e, BITS16);
	space->add_register("%bridge_ctrl", reg_ptr);

	scope = new_pp_scope();

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 0, PP_MASK(1), 0);
	scope->add_field("perr", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 1, PP_MASK(1), 1);
	scope->add_field("serr", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 2, PP_MASK(1), 2);
	scope->add_field("isa", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 3, PP_MASK(1), 3);
	scope->add_field("vga", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 4, PP_MASK(1), 4);
	scope->add_field("vga16", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 5, PP_MASK(1), 5);
	scope->add_field("master_abort", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 6, PP_MASK(1), 6);
	scope->add_field("sec_bus_reset", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 7, PP_MASK(1), 7);
	scope->add_field("fbb", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 8, PP_MASK(1), 8);
	scope->add_field("pri_timer", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 9, PP_MASK(1), 9);
	scope->add_field("sec_timer", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 10, PP_MASK(1), 10);
	scope->add_field("discard_timer", field_ptr);

	field_ptr = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field_ptr->add_regbits(reg_ptr.get(), 11, PP_MASK(1), 11);
	scope->add_field("discard_timer_serr", field_ptr);

	space->add_scope("bridge_ctrl", scope);
}

// DEVICE MACRO
//FIXME: move this back inline?
void
create_device(pp_space *space)
{
	pp_direct_field_ptr field_ptr;
	pp_register_ptr reg_ptr;
	const pp_binding *binding = space->binding();
	pp_scope_ptr scope_ptr;

	const pp_field *bar_type;
	const pp_field *bar_width;

	//   if bar1 is not defined, bar0 must be 64 bit, bar2 is ok.
	//   if bar1.type is not 64 bit memory, bar2 is ok.
	//if (!defined(bar1) ||
	//    !(bar1.type == MEM && bar1.width == BITS64)) {
	//	BAR(bar2, 0x18)
	if (!dirent_defined(space, pp_path("bar1"))) {
		base_address_register("bar2", space, binding, 0x18);
	} else {
		bar_type = get_field(space, pp_path("bar1/type"));
		bar_width = get_field(space, pp_path("bar1/width"));
		if (!(bar_type->read() == bar_type->lookup("mem")
		   && bar_width->read() == bar_width->lookup("bits64"))) {
			base_address_register("bar2", space, binding, 0x18);
		}
	}

	//if (!defined(bar2) ||
	//    !(bar2.type == MEM && bar2.width == BITS64)) {
	//	BAR(bar3, 0x1c)
	if (!dirent_defined(space, pp_path("bar2"))) {
		base_address_register("bar3", space, binding, 0x1c);
	} else {
		bar_type = get_field(space, pp_path("bar2/type"));
		bar_width = get_field(space, pp_path("bar2/width"));
		if (!(bar_type->read() == bar_type->lookup("mem")
		   && bar_width->read() == bar_width->lookup("bits64"))) {
			base_address_register("bar3", space, binding, 0x1c);
		}
	}

	//if (!defined(bar3) ||
	//    !(bar3.type == MEM && bar3.width == BITS64)) {
	//	BAR(bar4, 0x20)
	if (!dirent_defined(space, pp_path("bar3"))) {
		base_address_register("bar4", space, binding, 0x20);
	} else {
		bar_type = get_field(space, pp_path("bar3/type"));
		bar_width = get_field(space, pp_path("bar3/width"));
		if (!(bar_type->read() == bar_type->lookup("mem")
		   && bar_width->read() == bar_width->lookup("bits64"))) {
			base_address_register("bar4", space, binding, 0x20);
		}
	}

	//if (!defined(bar4) ||
	//    !(bar4.type == MEM && bar4.width == BITS64)) {
	//	BAR(bar5, 0x24)
	if (!dirent_defined(space, pp_path("bar4"))) {
		base_address_register("bar5", space, binding, 0x24);
	} else {
		bar_type = get_field(space, pp_path("bar4/type"));
		bar_width = get_field(space, pp_path("bar4/width"));
		if (!(bar_type->read() == bar_type->lookup("mem")
		   && bar_width->read() == bar_width->lookup("bits64"))) {
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

	// CAPABILITIES
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
	scope->add_field("cap66", field_ptr);
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
	anon_enum->add_value("med", 1);
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
	const pp_field *bar_type = get_field(space, pp_path("bar0/type"));
	const pp_field *bar_width = get_field(space, pp_path("bar0/width"));

	//if (!(bar0.type == MEM && bar0.width == BITS64)) {
	//	BAR(bar1, 0x14)
	//}
	if (!(bar_type->read() == bar_type->lookup("mem")
	   && bar_width->read() == bar_width->lookup("bits64"))) {
		base_address_register("bar1", space, binding, 0x14);
	}

	// if (hdrtype.type == DEVICE) {
	field = get_field(space, pp_path("hdrtype/type"));
	if (field->read() == field->lookup("device")) {
		create_device(space);
	} else if (field->read() == field->lookup("pci_bridge")) {
		create_pci_bridge(space);
	} else if (field->read() == field->lookup("card_bridge")) {
		//FIXME:
	}

	return space_ptr;
}

#if 0
		//FIXME: left off here
	} else if (hdrtype.type == CARD_BRIDGE) {
		//FIXME:
	}
}
<---- pci.pp
#endif

using namespace std;

void
dump_scope(const pp_scope *scope, string indent = "")
{
	for (size_t i = 0; i < scope->n_datatypes(); i++) {
		cout << indent;
		cout << "datatype: "
		     << scope->datatype_name(i) << endl;
	}
	for (size_t i = 0; i < scope->n_dirents(); i++) {
		if (scope->dirent(i)->is_register()) {
			cout << indent;
			cout << "register: "
			     << scope->dirent_name(i)
			     << ": (0x"
			     << std::hex
			     << pp_register_from_dirent(
			        scope->dirent(i))->read()
			     << ")"
			     << endl;
		}
		if (scope->dirent(i)->is_field()) {
			cout << indent;
			cout << "field:   "
			     << scope->dirent_name(i)
			     << ": "
			     << pp_field_from_dirent(
			        scope->dirent(i))->evaluate()
			     << " (0x"
			     << std::hex
			     << pp_field_from_dirent(
			        scope->dirent(i))->read()
			     << ")"
			     << endl;
		}
		if (scope->dirent(i)->is_scope()) {
			cout << indent;
			cout << "scope:   "
			     << scope->dirent_name(i)
			     << endl;
			const pp_scope *sub = pp_scope_from_dirent(
			    scope->dirent(i));
			dump_scope(sub, indent+"    ");
		}
	}
}

void
dump_space(const pp_space *space, string indent = "")
{
	dump_scope(space, indent);
}

void
dump_device(const pp_device *dev, string indent = "")
{
	for (size_t i = 0; i < dev->n_datatypes(); i++) {
		cout << indent;
		cout << "datatype: "
		     << dev->datatype_name(i) << endl;
	}
	for (size_t i = 0; i < dev->n_dirents(); i++) {
		if (dev->dirent(i)->is_device()) {
			cout << indent;
			cout << "device: "
			     << dev->dirent_name(i)
			     << endl;
			dump_device(pp_device_from_dirent(dev->dirent(i)));
		}
		if (dev->dirent(i)->is_space()) {
			cout << indent;
			cout << "space:   "
			     << dev->dirent_name(i)
			     << endl;
			dump_space(pp_space_from_dirent(dev->dirent(i)),
			    indent+"    ");
		}
		if (dev->dirent(i)->is_field()) {
			cout << indent;
			cout << "field:   "
			     << dev->dirent_name(i)
			     << ": "
			     << pp_field_from_dirent(
			        dev->dirent(i))->evaluate()
			     << " (0x"
			     << std::hex
			     << pp_field_from_dirent(
			        dev->dirent(i))->read()
			     << ")"
			     << endl;
		}
	}
}

void
dump_platform(const pp_platform *platform, string indent = "")
{
	dump_device(platform, indent);
}

int
main()
{
	pp_platform_ptr platform = new_pp_platform();
	init_crap(platform);
	pp_driver_ptr driver = new_pci_driver(platform.get());
	driver->enumerate(platform.get());
	dump_platform(platform.get());

	pp_binding_ptr binding = new_pci_binding(pci_address(1,0,0));
	pp_space_ptr space = pci_generic_space(binding, platform.get());
	dump_scope(space.get());
	return 0;
}

