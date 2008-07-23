#include "pp.h"
#include "generic_device.h"
#include "fake_language.h"

// Functions for easy access to individual registers returned by cpuid
inline pp_regbits
EAX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi, lo);
}
inline pp_regbits
EAX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit);
}
inline pp_regbits
EBX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi + 32, lo + 32);
}
inline pp_regbits
EBX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit + 32);
}
inline pp_regbits
ECX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi + 64, lo + 64);
}
inline pp_regbits
ECX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit + 64);
}
inline pp_regbits
EDX(const string &name, const pp_bitwidth &hi, const pp_bitwidth &lo)
{
	return BITS(name, hi + 96, lo + 96);
}
inline pp_regbits
EDX(const string &name, const pp_bitwidth &bit)
{
	return BITS(name, bit + 96);
}

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
		if (FIELD_EQ("vendor", "intel")) {
			pp_value family = READ(EAX("%fn1", 11, 8))
			                + READ(EAX("%fn1", 27, 20));
			return family;
		} else if (FIELD_EQ("vendor", "amd")) {
			pp_value base_family = READ(EAX("%fn1", 11, 8));
			if (base_family == 0xf) {
				return READ(EAX("%fn1", 27, 20)) + 0xf;
			}
			return base_family;
		}
		WARN("unknown CPUID vendor, using base family");
		return READ(EAX("%fn1", 11, 8));
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
		if (FIELD_EQ("vendor", "intel")) {
			return READ(EAX("%fn1", 19, 16) + EAX("%fn1", 7, 4));
		} else if (FIELD_EQ("vendor", "amd")) {
			pp_value base_family = READ(EAX("%fn1", 11, 8));
			if (base_family == 0xf) {
				return READ(EAX("%fn1", 19, 16)
					    + EAX("%fn1", 7, 4));
			}
			return READ(EAX("%fn1", 7, 4));
		}
		WARN("unknown CPUID vendor, using base_model");
		return READ(EAX("%fn1", 7, 4));
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
	FIELD("largest_std_fn", "hex_t", EAX("%fn0", 31, 0));
	FIELD("vendor", "cpuid_vendor_t",
			ECX("%fn0", 31, 0) +
			EDX("%fn0", 31, 0) +
			EBX("%fn0", 31, 0));

	if (FIELD_GE("largest_std_fn", 1)) {
		REG128("%fn1", 1);
		FIELD("family", "int_t", PROCS(cpuid_family_procs));
		FIELD("model", "int_t", PROCS(cpuid_model_procs));
		FIELD("stepping", "int_t", EAX("%fn1", 3, 0));

		if (FIELD_EQ("vendor", "intel")) {
			FIELD("type", ANON_ENUM(
					KV("oem", 0),
					KV("overdrive", 1),
					KV("dual", 2)),
				EAX("%fn1", 13, 12));
		}

		if (FIELD_EQ("vendor", "amd")) {
			FIELD("std_features", "cpuid_features_t",
					ECX("%fn1", 31, 0)
					+ EDX("%fn1", 31, 0));

			FIELD("brand_id_8bit", "int_t", EBX("%fn1", 7, 0));
			FIELD("clflush_size", ANON_INT("QWORDs"),
					EBX("%fn1", 15, 8));
			if (FIELD_TEST("std_features", "htt")) {
				FIELD("logical_proc_count", "int_t",
						EBX("%fn1", 23, 16));
			} else {
				FIELD("logical_proc_count", "int_t", 1);
			}
			FIELD("init_lapic_id", "int_t", EBX("%fn1", 31, 24));
		}
	}

	//FIXME: more std functions

	#if 0 //FIXME: need something like this for other standard fns?
	for (int i = 0; i < 4096; i += 4) {
		REG32(to_string(boost::format("%%PCI.%04x") %i), i);
	}
	#endif
}
