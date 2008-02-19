#include "pp.h"
#include "utils.h"

void
CPUID_SCOPE(const string &name, const pp_value &cpu, const pp_value &func)
{
	OPEN_SCOPE(name, BIND("cpuid", ARGS(cpu, func)));
	REG32("%eax", 0);
	REG32("%ebx", 1);
	REG32("%ecx", 2);
	REG32("%edx", 3);
}

/* populate the current scope with generic CPUID fields */
class cpuid_family_procs: public proc_field_accessor
{
	pp_value
	read() const
	{
		pp_value base_family = READ(BITS("%eax", 11, 8));
		if (base_family == 0xf) {
			return READ(BITS("%eax", 27, 20)) + 0xf;
		}
		return base_family;
	}
	void
	write(const pp_value &value) const
	{
		// not supported
	}
};
class cpuid_model_procs: public proc_field_accessor
{
	pp_value
	read() const
	{
		pp_value base_family = READ(BITS("%eax", 11, 8));
		if (base_family == 0xf) {
			return READ(BITS("%eax", 19, 16) + BITS("%eax", 7, 4));
		}
		return READ(BITS("%eax", 7, 4));
	}
	void
	write(const pp_value &value) const
	{
		// not supported
	}
};
void
cpuid_generic_device(const pp_value &cpu)
{
	CPUID_SCOPE("cpuid.0", cpu, 0);
	FIELD("largest_std_fn", "hex_t", BITS("%eax", 31, 0));
	FIELD("vendor", "cpu_vendor_t",
			BITS("%ecx", 31, 0) +
			BITS("%edx", 31, 0) +
			BITS("%ebx", 31, 0));
	CLOSE_SCOPE();

	if (FIELD_GE("cpuid.0/largest_std_fn", 1)) {
		CPUID_SCOPE("cpuid.1" , cpu, 1);
		FIELD("family", "int_t", PROCS(new cpuid_family_procs));
		FIELD("model", "int_t", PROCS(new cpuid_model_procs));
		FIELD("stepping", "int_t", BITS("%eax", 3, 0));
		//FIXME: more
		CLOSE_SCOPE();
	}

	#if 0 //FIXME: need something like this?
	for (int i = 0; i < 4096; i += 4) {
		REG32(to_string(boost::format("%%PCI.%04x") %i), i);
	}
	#endif
}
