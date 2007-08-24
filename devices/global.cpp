#include "device_init.h"
#include "utils.h"
#include "pp_platform.h"
#include "pp_datatypes.h"

/*
 * When we get a real language, these things will be defined there.  Until
 * then, we define them here.
 */

/* call this when you init a new platform */
void
global_datatypes_init(pp_platform *platform)
{
	/* primitives */
	platform->add_datatype("int_t", new_pp_int());
	platform->add_datatype("hex_t", new_pp_hex());
	platform->add_datatype("hex8_t", new_pp_hex(BITS8));
	platform->add_datatype("hex16_t", new_pp_hex(BITS16));
	platform->add_datatype("hex32_t", new_pp_hex(BITS32));
	platform->add_datatype("hex64_t", new_pp_hex(BITS64));
	platform->add_datatype("addr16_t", new_pp_hex(BITS16));
	platform->add_datatype("addr32_t", new_pp_hex(BITS32));
	platform->add_datatype("addr64_t", new_pp_hex(BITS64));

	//FIXME: for now these will do.  a global add_datatype() template
	//function would be better.

	/* various booleans */
	pp_bool_ptr b;
	b = ANON_BOOL("yes", "no");
	platform->add_datatype("yesno_t", b);
	b = ANON_BOOL("true", "false");
	platform->add_datatype("truefalse_t", b);
	b = ANON_BOOL("on", "off");
	platform->add_datatype("onoff_t", b);
	b = ANON_BOOL("enabled", "disabled");
	platform->add_datatype("enabledisable_t", b);

	/* generic bitmask */
	pp_bitmask_ptr bm;
	bm = ANON_BITMASK("bit0", pp_value(0), "bit1", pp_value(1),
			"bit2", pp_value(2), "bit3", pp_value(3),
			"bit4", pp_value(4), "bit5", pp_value(5),
			"bit6", pp_value(6), "bit7", pp_value(7),
			"bit8", pp_value(8), "bit9", pp_value(9),
			"bit10", pp_value(10), "bit11", pp_value(11),
			"bit12", pp_value(12), "bit13", pp_value(13),
			"bit14", pp_value(14), "bit15", pp_value(15),
			"bit16", pp_value(16), "bit17", pp_value(17),
			"bit18", pp_value(18), "bit19", pp_value(19),
			"bit20", pp_value(20), "bit21", pp_value(21),
			"bit22", pp_value(22), "bit23", pp_value(23),
			"bit24", pp_value(24), "bit25", pp_value(25),
			"bit26", pp_value(26), "bit27", pp_value(27),
			"bit28", pp_value(28), "bit29", pp_value(29),
			"bit30", pp_value(30), "bit31", pp_value(31),
			"bit32", pp_value(32), "bit33", pp_value(33),
			"bit34", pp_value(34), "bit35", pp_value(35),
			"bit36", pp_value(36), "bit37", pp_value(37),
			"bit38", pp_value(38), "bit39", pp_value(71),
			"bit40", pp_value(40), "bit41", pp_value(41),
			"bit42", pp_value(42), "bit43", pp_value(43),
			"bit44", pp_value(44), "bit45", pp_value(45),
			"bit46", pp_value(46), "bit47", pp_value(47),
			"bit48", pp_value(48), "bit49", pp_value(49),
			"bit50", pp_value(50), "bit51", pp_value(51),
			"bit52", pp_value(52), "bit53", pp_value(53),
			"bit54", pp_value(54), "bit55", pp_value(55),
			"bit56", pp_value(56), "bit57", pp_value(57),
			"bit58", pp_value(58), "bit59", pp_value(59),
			"bit60", pp_value(60), "bit61", pp_value(61),
			"bit62", pp_value(62), "bit63", pp_value(63),
			NULL);
	platform->add_datatype("bitmask_t", bm);
}
