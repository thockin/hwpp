#include "pp.h"
#include "generic_device.h"
#include "fake_language.h"

void
CPUID_SCOPE(const string &name, const pp_value &cpu)
{
	OPEN_SCOPE(name, BIND("cpuid", ARGS(cpu)));
	BOOKMARK("cpuid");
	cpuid_generic_device();
}

// create a scope and simple fields for an "opaque" MSR
void
CPUID_REGSCOPE(const string &name, const pp_value &address)
{
	string regname = "%" + name;
	REG64(regname, address);
	OPEN_SCOPE(name);
	FIELD("addr", "hex64_t", address);
	FIELD("eax", "hex32_t", BITS(regname, 31, 0));
	FIELD("ebx", "hex32_t", BITS(regname, 63, 32));
	FIELD("ecx", "hex32_t", BITS(regname, 95, 64));
	FIELD("edx", "hex32_t", BITS(regname, 127, 96));
	CLOSE_SCOPE();
}

/* populate the current scope with generic CPUID fields */
class cpuid_family_procs: public pp_rwprocs
{
	pp_value
	read() const
	{
		pp_value base_family = READ(BITS("%fn1", 11, 8));
		if (base_family == 0xf) {
			return READ(BITS("%fn1", 27, 20)) + 0xf;
		}
		return base_family;
	}
	void
	write(const pp_value &value) const
	{
		// not supported
	}
};
class cpuid_model_procs: public pp_rwprocs
{
	pp_value
	read() const
	{
		pp_value base_family = READ(BITS("%fn1", 11, 8));
		if (base_family == 0xf) {
			return READ(BITS("%fn1", 19, 16) + BITS("%fn1", 7, 4));
		}
		return READ(BITS("%fn1", 7, 4));
	}
	void
	write(const pp_value &value) const
	{
		// not supported
	}
};
void
cpuid_generic_device()
{
	REG128("%fn0", 0);
	FIELD("largest_std_fn", "hex_t", BITS("%fn0", 31, 0));
	FIELD("vendor", "cpuid_vendor_t",
			BITS("%fn0", 95, 64) +
			BITS("%fn0", 127, 96) +
			BITS("%fn0", 63, 32));

	if (FIELD_GE("largest_std_fn", 1)) {
		REG128("%fn1", 1);
		FIELD("family", "int_t", PROCS(cpuid_family_procs));
		FIELD("model", "int_t", PROCS(cpuid_model_procs));
		FIELD("stepping", "int_t", BITS("%fn1", 3, 0));

		FIELD("std_features", "cpuid_features_t",
				BITS("%fn1", 95, 64) + BITS("%fn1", 127, 96));

		FIELD("brand_id_8bit", "int_t", BITS("%fn1", 39, 32));
		FIELD("clflush_size", ANON_INT("QWORDs"), BITS("%fn1", 47, 40));
		if (FIELD_TEST("std_features", "htt")) {
			FIELD("logical_proc_count", "int_t",
					BITS("%fn1", 55, 48));
		} else {
			FIELD("logical_proc_count", "int_t", 1);
		}
		FIELD("init_lapic_id", "int_t", BITS("%fn1", 63, 56));
	}

	//FIXME: more std functions

	#if 0 //FIXME: need something like this for other standard fns?
	for (int i = 0; i < 4096; i += 4) {
		REG32(to_string(boost::format("%%PCI.%04x") %i), i);
	}
	#endif
}
