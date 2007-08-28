#include "device_init.h"
#include "utils.h"
#include "pp_platform.h"
#include "pp_datatypes.h"

/*
 * When we get a real language, these things will be defined there.  Until
 * then, we define them here.
 */

int force_devices_linkage;

/* call this when you init a new platform */
//FIXME: do this automagically via a static object ctor?
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
	bm = ANON_BITMASK(
			{"bit0",  0},  {"bit1",  1},
			{"bit2",  2},  {"bit3",  3},
			{"bit4",  4},  {"bit5",  5},
			{"bit6",  6},  {"bit7",  7},
			{"bit8",  8},  {"bit9",  9},
			{"bit10", 10}, {"bit11", 11},
			{"bit12", 12}, {"bit13", 13},
			{"bit14", 14}, {"bit15", 15},
			{"bit16", 16}, {"bit17", 17},
			{"bit18", 18}, {"bit19", 19},
			{"bit20", 20}, {"bit21", 21},
			{"bit22", 22}, {"bit23", 23},
			{"bit24", 24}, {"bit25", 25},
			{"bit26", 26}, {"bit27", 27},
			{"bit28", 28}, {"bit29", 29},
			{"bit30", 30}, {"bit31", 31},
			{"bit32", 32}, {"bit33", 33},
			{"bit34", 34}, {"bit35", 35},
			{"bit36", 36}, {"bit37", 37},
			{"bit38", 38}, {"bit39", 39},
			{"bit40", 40}, {"bit41", 41},
			{"bit42", 42}, {"bit43", 43},
			{"bit44", 44}, {"bit45", 45},
			{"bit46", 46}, {"bit47", 47},
			{"bit48", 48}, {"bit49", 49},
			{"bit50", 50}, {"bit51", 51},
			{"bit52", 52}, {"bit53", 53},
			{"bit54", 54}, {"bit55", 55},
			{"bit56", 56}, {"bit57", 57},
			{"bit58", 58}, {"bit59", 59},
			{"bit60", 60}, {"bit61", 61},
			{"bit62", 62}, {"bit63", 63}
		);
	platform->add_datatype("bitmask_t", bm);
}
