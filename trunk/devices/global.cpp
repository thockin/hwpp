#include "device_init.h"
#include "fake_language.h"
#include "pp_datatypes.h"

/*
 * When we get a real language, these things will be defined there.  Until
 * then, we define them here.
 */

int force_devices_linkage;

/* call this when you init a new platform */
void
platform_global_init()
{
	/* primitives */
	INT("int_t");
	HEX("hex_t");
	HEX("hex4_t", BITS4);
	HEX("hex8_t", BITS8);
	HEX("hex12_t", BITS12);
	HEX("hex16_t", BITS16);
	HEX("hex20_t", BITS20);
	HEX("hex32_t", BITS32);
	HEX("hex64_t", BITS64);
	HEX("hex128_t", BITS128);
	HEX("addr16_t", BITS16);
	HEX("addr32_t", BITS32);
	HEX("addr64_t", BITS64);

	/* various booleans */
	BOOL("yesno_t", "yes", "no");
	BOOL("truefalse_t", "true", "false");
	BOOL("onoff_t", "on", "off");
	BOOL("enabledisable_t", "enabled", "disabled");

	/* generic bitmask */
	BITMASK("bitmask_t",
			KV("bit0",  0),  KV("bit1",  1),
			KV("bit2",  2),  KV("bit3",  3),
			KV("bit4",  4),  KV("bit5",  5),
			KV("bit6",  6),  KV("bit7",  7),
			KV("bit8",  8),  KV("bit9",  9),
			KV("bit10", 10), KV("bit11", 11),
			KV("bit12", 12), KV("bit13", 13),
			KV("bit14", 14), KV("bit15", 15),
			KV("bit16", 16), KV("bit17", 17),
			KV("bit18", 18), KV("bit19", 19),
			KV("bit20", 20), KV("bit21", 21),
			KV("bit22", 22), KV("bit23", 23),
			KV("bit24", 24), KV("bit25", 25),
			KV("bit26", 26), KV("bit27", 27),
			KV("bit28", 28), KV("bit29", 29),
			KV("bit30", 30), KV("bit31", 31),
			KV("bit32", 32), KV("bit33", 33),
			KV("bit34", 34), KV("bit35", 35),
			KV("bit36", 36), KV("bit37", 37),
			KV("bit38", 38), KV("bit39", 39),
			KV("bit40", 40), KV("bit41", 41),
			KV("bit42", 42), KV("bit43", 43),
			KV("bit44", 44), KV("bit45", 45),
			KV("bit46", 46), KV("bit47", 47),
			KV("bit48", 48), KV("bit49", 49),
			KV("bit50", 50), KV("bit51", 51),
			KV("bit52", 52), KV("bit53", 53),
			KV("bit54", 54), KV("bit55", 55),
			KV("bit56", 56), KV("bit57", 57),
			KV("bit58", 58), KV("bit59", 59),
			KV("bit60", 60), KV("bit61", 61),
			KV("bit62", 62), KV("bit63", 63));
}
