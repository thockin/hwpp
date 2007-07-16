#include <iostream>

#include "pp.h"
#include "pp_space.h"
#include "pp_register.h"
#include "pp_datatypes.h"
#include "pp_fields.h"
#include "pci_driver.h"
#include "pci_io.h"
#include "pci_binding.h"

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

	platform->add_datatype("hex8_t", new_pp_hex(BITS8));
	platform->add_datatype("hex16_t", new_pp_hex(BITS16));
	platform->add_datatype("hex32_t", new_pp_hex(BITS32));

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
	e->add_value("data acquisitiona / signal processing controller", 0x11);
	//TODO: incomplete list
	e->set_default("unknown class");
	platform->add_datatype("pci_class_t", e);
}

void
regfield(string name, pp_scope_ptr scope, pp_binding_ptr binding,
    pp_regaddr address, pp_bitwidth width, pp_const_datatype_ptr type,
    pp_register_ptr *preg, pp_direct_field_ptr *pfield)
{
	*preg = new_pp_register(binding, address, width);
	scope->add_register("%" + name, *preg);
	*pfield = new_pp_direct_field(type);
	(*pfield)->add_regbits(*preg, 0, PP_MASK(width), 0);
	scope->add_field(name, *pfield);
}

pp_space_ptr
pci_generic_space(pp_binding_ptr binding)
{
	pp_space_ptr space = new_pp_space(binding);
	pp_register_ptr reg;
	pp_direct_field_ptr field;
	pp_enum_ptr anon_enum;
	pp_int_ptr anon_int;
	pp_bitmask_ptr anon_bitmask;
	pp_scope_ptr scope;

	// regrange32 PCI(0, 4096, 4, PRESERVE);
	for (int i = 0; i < 4096; i += 4) {
		pp_register_ptr reg = new_pp_register(binding, i, BITS32);
		space->add_register(to_string(
		    boost::format("%%PCI[%04x]") %i), reg);
	}

	// regfield16 vendor(0x00, PCI_VENDOR);
	regfield("vendor", space, binding, 0x00, BITS16,
	    space->resolve_datatype("pci_vendor_t"), &reg, &field);

	// regfield16 device(0x02, HEX16);
	regfield("device", space, binding, 0x02, BITS16,
	    space->resolve_datatype("hex16_t"), &reg, &field);

	// reg16 command(0x04, preserve);
	reg = new_pp_register(binding, 0x04, BITS16);
	space->add_register("%command", reg);

	// scope command {
	scope = new_pp_scope();
	//	field io(command[0], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 0, PP_MASK(1), 0);
	scope->add_field("io", field);
	//	field mem(command[1], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 1, PP_MASK(1), 0);
	scope->add_field("mem", field);
	//	field bm(command[2], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 2, PP_MASK(1), 0);
	scope->add_field("bm", field);
	//	field special(command[3], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 3, PP_MASK(1), 0);
	scope->add_field("special", field);
	//	field mwinv(command[4], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 4, PP_MASK(1), 0);
	scope->add_field("mwinv", field);
	//	field vgasnoop(command[5], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 5, PP_MASK(1), 0);
	scope->add_field("vgasnoop", field);
	//	field perr(command[6], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 6, PP_MASK(1), 0);
	scope->add_field("perr", field);
	//	field step(command[7], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 7, PP_MASK(1), 0);
	scope->add_field("step", field);
	//	field serr(command[8], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 8, PP_MASK(1), 0);
	scope->add_field("serr", field);
	//	field fbb(command[9], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 9, PP_MASK(1), 0);
	scope->add_field("fbb", field);
	//	field intr(command[10], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 10, PP_MASK(1), 0);
	scope->add_field("intr", field);
	//}
	space->add_scope("command", scope);

	// reg16 status(0x06, preserve);
	reg = new_pp_register(binding, 0x06, BITS16);
	space->add_register("%status", reg);

	// scope status {
	scope = new_pp_scope();
	//	field intr(status[3], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 3, PP_MASK(1), 0);
	scope->add_field("intr", field);
	//	field caps(status[4], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 4, PP_MASK(1), 0);
	scope->add_field("caps", field);
	//	field cap66(status[5], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 5, PP_MASK(1), 0);
	scope->add_field("cap66", field);
	//	field fbb(status[7], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 7, PP_MASK(1), 0);
	scope->add_field("fbb", field);
	//	field mdperr(status[8], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 8, PP_MASK(1), 0);
	scope->add_field("mdperr", field);
	//	field devsel(status[10:9], enum() {
	//		FAST = 0
	//		MEDIUM = 1
	//		SLOW = 2
	//	});
	anon_enum = new_pp_enum();
	anon_enum->add_value("fast", 0);
	anon_enum->add_value("med", 1);
	anon_enum->add_value("slow", 2);
	field = new_pp_direct_field(anon_enum);
	field->add_regbits(reg, 9, PP_MASK(2), 0);
	scope->add_field("devsel", field);
	//	field sigtabort(status[11], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 11, PP_MASK(1), 0);
	scope->add_field("sigtabort", field);
	//	field rcvtabort(status[12], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 12, PP_MASK(1), 0);
	scope->add_field("rcvtabort", field);
	//	field rcvmabort(status[13], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 13, PP_MASK(1), 0);
	scope->add_field("rcvmabort", field);
	//	field serr(status[14], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 14, PP_MASK(1), 0);
	scope->add_field("serr", field);
	//	field perr(status[15], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 15, PP_MASK(1), 0);
	scope->add_field("perr", field);
	//}
	space->add_scope("status", scope);

	//regfield8 class(0x0b, PCI_CLASS);
	regfield("class", space, binding, 0x0b, BITS8,
	    space->resolve_datatype("pci_class_t"), &reg, &field);

	//regfield8 subclass(0x0a, ...); // per-class subclasses
	pp_enum_ptr subclass_type = new_pp_enum();
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
	regfield("subclass", space, binding, 0x0a, BITS8, subclass_type,
	    &reg, &field);

//FIXME: would be better to have detailed progintfs
	//regfield8 progintf(0x09, HEX8);
	reg = new_pp_register(binding, 0x09, BITS8);
	space->add_register("%progintf", reg);
	field = new_pp_direct_field(space->resolve_datatype("hex8_t"));
	field->add_regbits(reg, 0, PP_MASK(BITS8), 0);
	space->add_field("progintf", field);

	//regfield8 revision(0x08, HEX8);
	regfield("revision", space, binding, 0x08, BITS8,
	    space->resolve_datatype("hex8_t"), &reg, &field);

	//regfield8 cacheline(0x0c, int("dwords"));
	anon_int = new_pp_int("DWORDs");
	regfield("cacheline", space, binding, 0x0c, BITS8, anon_int,
	    &reg, &field);

	//regfield8 latency(0x0d, int("clocks"));
	anon_int = new_pp_int("clocks");
	regfield("latency", space, binding, 0x0d, BITS8, anon_int,
	    &reg, &field);

	//reg8 hdrtype(0x0e, preserve);
	reg = new_pp_register(binding, 0x0e, BITS8);
	space->add_register("%hdrtype", reg);

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
	field = new_pp_direct_field(anon_enum);
	field->add_regbits(reg, 0, PP_MASK(7), 0);
	scope->add_field("type", field);
	//	field multi_function(hdrtype[7], yes_no);
	field = new_pp_direct_field(space->resolve_datatype("yesno_t"));
	field->add_regbits(reg, 7, PP_MASK(1), 0);
	scope->add_field("multi_function", field);
	//}
	space->add_scope("hdrtype", scope);

	return space;
}

#if 0
	reg8 bist(0x0f, preserve);
	scope bist {
		field capable(bist[7], yes_no);
		field start(bist[6], yes_no);
		field code(bist[3:0], hex8);
	}

	regfield8 intline(0x3c, int);
	regfield8 intpin(0x3c, enum() {
		NONE = 0
		INTA = 1
		INTB = 2
		INTC = 3
		INTD = 4
	    });

	// All BARs look like this.
	macro BAR(name, offset) {
	    scope name {
		//FIXME: unless regs have a hierarchy, we had a dup name
		//       maybe name the reg $name - then what is "upper" ?
		reg32 lower(offset, preserve);
		field type(lower[0], enum() {
			MEM = 0
			IO = 1
		    });
		if (type == MEM) {
			field width(lower[2:1], enum() {
				BITS32 = 0,
				BITS20 = 1,
				BITS64 = 2,
			});
			field prefetch(lower[3], yes_no);
		}

		if (type == IO) {
			field addr((lower[15:2],zero[1:0]), ADDR16);
		} else if (width == BITS32 || width == BITS20) {
			field addr((lower[31:4],zero[3:0]), ADDR32);
		} else {
			reg32 upper(offset+4, preserve);
			field addr((upper[31:0],lower[31:4],zero[3:0]),
				ADDR64);
		}
	    }
	}

	// All devices have at least 2 BARs.
	BAR(bar0, 0x10) {
	if (!(bar0.type == MEM && bar0.width == BITS64)) {
		BAR(bar1, 0x14)
	}
	if (hdrtype == DEVICE) {
		// Figure out the next BAR definition:
		//   if bar1 is not defined, bar0 must be 64 bit, bar2 is ok.
		//   if bar1.type is not 64 bit memory, bar2 is ok.
		if (!defined(bar1) ||
		    !(bar1.type == MEM && bar1.width == BITS64)) {
			BAR(bar2, 0x18)
		}
		if (!defined(bar2) ||
		    !(bar2.type == MEM && bar2.width == BITS64)) {
			BAR(bar3, 0x1c)
		}
		if (!defined(bar3) ||
		    !(bar3.type == MEM && bar3.width == BITS64)) {
			BAR(bar4, 0x20)
		}
		if (!defined(bar4) ||
		    !(bar4.type == MEM && bar4.width == BITS64)) {
			BAR(bar5, 0x24)
		}

		regfield32 cisptr(0x28, ADDR32);

		regfield16 subvendor(0x2c, PCI_VENDOR);
		regfield16 subdevice(0x2e, HEX16);

		// Handle the PCI capabilities linked-list.
		//FIXME: using $ here really helps.  Maybe when you read a
		//field/var/method you use $?
		if (status.caps) {
			regfield8 capptr(0x34, HEX8);
			var $ptr = capptr;
			var $i = 0;
			while ($ptr != 0) {
				//FIXME: don't use [] here - it's for bits
				scope capability[$i] {
					regfield8 id($ptr, HEX8);
					regfield8 next($ptr+1, HEX8);
					//rest of caps fields
				}
				$ptr = capability[$i].next;
				$i = $i+1;
			}
		}

		reg32 rombase(0x30, preserve);
		scope rombase {
			field en(rombase[0], yes_no);
			field addr(rombase[31:11], HEX32);
		}

		regfield8 mingnt(0x3e, INT("1/4 usec"));
		regfield8 maxlat(0x3f, INT("1/4 usec"));
	} else if (hdrtype == PCI_BRIDGE) {
		regfield8 pri_bus(0x18, INT());
		regfield8 sec_bus(0x19, INT());
		regfield8 sub_bus(0x1a, INT());
		regfield8 sec_latency(0x1b, INT("clocks"));
		scope io_window {
			reg8 base_lo(1c, preserve);
			reg8 limit_lo(0x1d, preserve);
			reg16 base_hi(0x30, preserve);
			reg16 limit_hi(0x32, preserve);

			field width(base_lo[3:0], enum() {
				BITS16 = 0;
				BITS32 = 1;
			    });
			if (width == BITS16) {
				field base((base_lo[7:4],zero[11:0]},
					ADDR16));
				field limit((limit_lo[7:4],one[11:0]},
					ADDR16));
			} else if (width == BITS32) {
				field base(
					(base_hi[],base_lo[7:4],zero[11:0]),
					ADDR32);
				field limit(
					(limit_hi[],limit_lo[15:12],one[11:0]),
					ADDR32);
			}
		}
		//FIXME: left off here
	} else if (hdrtype == CARD_BRIDGE) {
		//FIXME:
	}
}
<---- pci.pp
#endif

using namespace std;

void
dump_scope(const pp_scope_ptr &scope, string indent = "")
{
	for (size_t i = 0; i < scope->datatypes().size(); i++) {
		cout << indent;
		cout << "datatype: "
		     << scope->datatypes().key_at(i) << endl;
	}
	for (size_t i = 0; i < scope->dirents.size(); i++) {
		if (scope->dirents[i].is_register()) {
			cout << indent;
			cout << "register: "
			     << scope->dirents.key_at(i)
			     << ": (0x"
			     << std::hex
			     << scope->dirents[i].as_register()->read()
			     << ")"
			     << endl;
		}
		if (scope->dirents[i].is_field()) {
			cout << indent;
			cout << "field:   "
			     << scope->dirents.key_at(i)
			     << ": "
			     << scope->dirents[i].as_field()->evaluate()
			     << " (0x"
			     << std::hex
			     << scope->dirents[i].as_field()->read()
			     << ")"
			     << endl;
		}
		if (scope->dirents[i].is_scope()) {
			cout << indent;
			cout << "scope:   " << scope->dirents.key_at(i) << endl;
			pp_scope_ptr sub = scope->dirents[i].as_scope();
			dump_scope(sub, indent+"    ");
		}
	}
}

void
dump_space(const pp_space_ptr &space, string indent = "")
{
	pp_scope_ptr scope = space;
	dump_scope(scope, indent);
}

void
dump_device(const pp_device_ptr &dev, string indent = "")
{
	for (size_t i = 0; i < dev->datatypes().size(); i++) {
		cout << indent;
		cout << "datatype: "
		     << dev->datatypes().key_at(i) << endl;
	}
	for (size_t i = 0; i < dev->dirents.size(); i++) {
		if (dev->dirents[i].is_device()) {
			cout << indent;
			cout << "device: "
			     << dev->dirents.key_at(i)
			     << endl;
			dump_device(dev->dirents[i].as_device());
		}
		if (dev->dirents[i].is_space()) {
			cout << indent;
			cout << "space:   " << dev->dirents.key_at(i) << endl;
			dump_space(dev->dirents[i].as_space(), indent+"    ");
		}
		if (dev->dirents[i].is_field()) {
			cout << indent;
			cout << "field:   "
			     << dev->dirents.key_at(i)
			     << ": "
			     << dev->dirents[i].as_field()->evaluate()
			     << " (0x"
			     << std::hex
			     << dev->dirents[i].as_field()->read()
			     << ")"
			     << endl;
		}
	}
}

void
dump_platform(const pp_platform_ptr &plat, string indent = "")
{
	pp_device_ptr dev = plat;
	dump_device(dev, indent);
}

int
main()
{
	pp_platform_ptr platform = new_pp_platform();
	init_crap(platform);
	pp_driver_ptr driver = new_pci_driver();
	driver->enumerate(platform);
	dump_platform(platform);

	pp_binding_ptr binding = new_pci_binding(pci_address(0,0,0));
	pp_space_ptr space = pci_generic_space(binding);
	dump_scope(space);
	return 0;
}

