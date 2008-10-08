#include "pp.h"
#include "generic_device.h"
#include "fake_language.h"

// create a new MSR-bound scope with generic fields
void
MSR_SCOPE(const string &name, const pp_value &cpu)
{
	OPEN_SCOPE(name, BIND("msr", ARGS(cpu)));
	BOOKMARK("msr");
	msr_generic_device();
}

// create a scope and simple fields for an "opaque" MSR
void
MSR_REGSCOPE(const string &name, const pp_value &address)
{
	string regname = "%" + name;
	REG64(regname, address);
	OPEN_SCOPE(name);
	FIELD("addr", "hex32_t", address);
	FIELD("data", "hex64_t", BITS(regname));
}

// populate the current scope with generic MSR device fields
void
msr_generic_device()
{
	/* timestamp counter */
	FIELD("TSC", "hex64_t", REG64(0x10));

	OPEN_SCOPE("apic");

	REG64("%APIC_BASE", 0x1b);
	FIELD("BSP", "yesno_t", BITS("%APIC_BASE", 8));
	FIELD("ApicEn", "yesno_t", BITS("%APIC_BASE", 11));
	FIELD("ApicBar", "addr64_t",
			BITS("%APIC_BASE", 39, 12) + BITS("%0", 11, 0));

	REG64("%EBL_CR_POWERON", 0x2a);
	FIELD("ApicClusterID", "int_t", BITS("%EBL_CR_POWERON", 17, 16));

	CLOSE_SCOPE(); // apic

	FIELD("PATCH_LEVEL", "hex64_t", REG64(0x8b));

	//FIXME: need to wrap this with a CPUID check for MTRR capability
	OPEN_SCOPE("mtrr");

	REG64("%MTRRCap", 0xfe);
	FIELD("MtrrCapFix", "yesno_t", BITS("%MTRRCap", 8));
	FIELD("MtrrCapVCnt", "int_t", BITS("%MTRRCap", 7, 0));
	FIELD("MtrrCapWc", "yesno_t", BITS("%MTRRCap", 10));

	REG64("%MTRRDefType", 0x2ff);
	FIELD("MTRRDefTypeFixEn", "enabledisable_t", BITS("%MTRRDefType", 10));
	FIELD("MTRRDefTypeEn", "enabledisable_t", BITS("%MTRRDefType", 11));
	FIELD("MTRRDefMemType", "hex8_t", BITS("%MTRRDefType", 7, 0));

	OPEN_SCOPE("fixed");

	REG64("%MTRRFix64K_00000", 0x250);
	FIELD("MtrrFix64k0xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 7, 0));
	FIELD("MtrrFix64k1xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 15, 8));
	FIELD("MtrrFix64k2xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 23, 16));
	FIELD("MtrrFix64k3xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 31, 24));
	FIELD("MtrrFix64k4xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 39, 32));
	FIELD("MtrrFix64k5xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 47, 40));
	FIELD("MtrrFix64k6xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 55, 48));
	FIELD("MtrrFix64k7xxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix64K_00000", 63, 56));

	REG64("%MTRRFix16K_80000", 0x258);
	FIELD("MtrrFix16k80xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 7, 0));
	FIELD("MtrrFix16k84xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 15, 8));
	FIELD("MtrrFix16k88xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 23, 16));
	FIELD("MtrrFix16k8CxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 31, 24));
	FIELD("MtrrFix16k90xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 39, 32));
	FIELD("MtrrFix16k94xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 47, 40));
	FIELD("MtrrFix16k98xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 55, 48));
	FIELD("MtrrFix16k9CxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_80000", 63, 56));

	REG64("%MTRRFix16K_A0000", 0x259);
	FIELD("MtrrFix16kA0xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 7, 0));
	FIELD("MtrrFix16kA4xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 15, 8));
	FIELD("MtrrFix16kA8xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 23, 16));
	FIELD("MtrrFix16kACxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 31, 24));
	FIELD("MtrrFix16kB0xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 39, 32));
	FIELD("MtrrFix16kB4xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 47, 40));
	FIELD("MtrrFix16kB8xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 55, 48));
	FIELD("MtrrFix16kBCxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix16K_A0000", 63, 56));

	REG64("%MTRRFix4K_C0000", 0x268);
	FIELD("MtrrFix4kC0xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 7, 0));
	FIELD("MtrrFix4kC1xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 15, 8));
	FIELD("MtrrFix4kC2xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 23, 16));
	FIELD("MtrrFix4kC3xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 31, 24));
	FIELD("MtrrFix4kC4xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 39, 32));
	FIELD("MtrrFix4kC5xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 47, 40));
	FIELD("MtrrFix4kC6xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 55, 48));
	FIELD("MtrrFix4kC7xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C0000", 63, 56));

	REG64("%MTRRFix4K_C8000", 0x269);
	FIELD("MtrrFix4kC8xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 7, 0));
	FIELD("MtrrFix4kC9xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 15, 8));
	FIELD("MtrrFix4kCAxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 23, 16));
	FIELD("MtrrFix4kCBxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 31, 24));
	FIELD("MtrrFix4kCCxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 39, 32));
	FIELD("MtrrFix4kCDxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 47, 40));
	FIELD("MtrrFix4kCExxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 55, 48));
	FIELD("MtrrFix4kCFxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_C8000", 63, 56));

	REG64("%MTRRFix4K_D0000", 0x26a);
	FIELD("MtrrFix4kD0xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 7, 0));
	FIELD("MtrrFix4kD1xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 15, 8));
	FIELD("MtrrFix4kD2xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 23, 16));
	FIELD("MtrrFix4kD3xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 31, 24));
	FIELD("MtrrFix4kD4xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 39, 32));
	FIELD("MtrrFix4kD5xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 47, 40));
	FIELD("MtrrFix4kD6xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 55, 48));
	FIELD("MtrrFix4kD7xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D0000", 63, 56));

	REG64("%MTRRFix4K_D8000", 0x26b);
	FIELD("MtrrFix4kD8xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 7, 0));
	FIELD("MtrrFix4kD9xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 15, 8));
	FIELD("MtrrFix4kDAxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 23, 16));
	FIELD("MtrrFix4kDBxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 31, 24));
	FIELD("MtrrFix4kDCxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 39, 32));
	FIELD("MtrrFix4kDDxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 47, 40));
	FIELD("MtrrFix4kDExxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 55, 48));
	FIELD("MtrrFix4kDFxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_D8000", 63, 56));

	REG64("%MTRRFix4K_E0000", 0x26c);
	FIELD("MtrrFix4kE0xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 7, 0));
	FIELD("MtrrFix4kE1xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 15, 8));
	FIELD("MtrrFix4kE2xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 23, 16));
	FIELD("MtrrFix4kE3xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 31, 24));
	FIELD("MtrrFix4kE4xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 39, 32));
	FIELD("MtrrFix4kE5xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 47, 40));
	FIELD("MtrrFix4kE6xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 55, 48));
	FIELD("MtrrFix4kE7xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E0000", 63, 56));

	REG64("%MTRRFix4K_E8000", 0x26d);
	FIELD("MtrrFix4kE8xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 7, 0));
	FIELD("MtrrFix4kE9xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 15, 8));
	FIELD("MtrrFix4kEAxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 23, 16));
	FIELD("MtrrFix4kEBxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 31, 24));
	FIELD("MtrrFix4kECxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 39, 32));
	FIELD("MtrrFix4kEDxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 47, 40));
	FIELD("MtrrFix4kEExxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 55, 48));
	FIELD("MtrrFix4kEFxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_E8000", 63, 56));

	REG64("%MTRRFix4K_F0000", 0x26e);
	FIELD("MtrrFix4kF0xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 7, 0));
	FIELD("MtrrFix4kF1xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 15, 8));
	FIELD("MtrrFix4kF2xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 23, 16));
	FIELD("MtrrFix4kF3xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 31, 24));
	FIELD("MtrrFix4kF4xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 39, 32));
	FIELD("MtrrFix4kF5xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 47, 40));
	FIELD("MtrrFix4kF6xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 55, 48));
	FIELD("MtrrFix4kF7xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F0000", 63, 56));

	REG64("%MTRRFix4K_F8000", 0x26f);
	FIELD("MtrrFix4kF8xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 7, 0));
	FIELD("MtrrFix4kF9xxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 15, 8));
	FIELD("MtrrFix4kFAxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 23, 16));
	FIELD("MtrrFix4kFBxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 31, 24));
	FIELD("MtrrFix4kFCxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 39, 32));
	FIELD("MtrrFix4kFDxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 47, 40));
	FIELD("MtrrFix4kFExxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 55, 48));
	FIELD("MtrrFix4kFFxxxMemType", "mtrr_type_t",
			BITS("%MTRRFix4K_F8000", 63, 56));

	CLOSE_SCOPE(); // fixed

	for (int i = 0; i < READ("MtrrCapVCnt"); i++) {
		OPEN_SCOPE("variable[]");
		REG64("%MTRRPhysBase", 0x200+(i*2));
		REG64("%MTRRPhysMask", 0x201+(i*2));
		FIELD("Valid", "yesno_t", BITS("%MTRRPhysMask", 11));
		if (FIELD_BOOL("Valid")) {
			FIELD("Type", "mtrr_type_t",
			    BITS("%MTRRPhysBase", 7, 0));
			FIELD("PhysBase", "addr64_t",
			    BITS("%MTRRPhysBase", 51, 12) + BITS("%0", 11, 0));
			FIELD("PhysMask", "hex64_t",
			    BITS("%MTRRPhysMask", 51, 12) + BITS("%0", 11, 0));
		}
		CLOSE_SCOPE(); // variable[]
	}

	CLOSE_SCOPE(); // mtrr

	OPEN_SCOPE("sysenter");

	REG64("%SYSENTER_CS", 0x174);
	REG64("%SYSENTER_ESP", 0x175);
	REG64("%SYSENTER_EIP", 0x176);

	FIELD("SYSENTER_CS", "hex16_t", BITS("%SYSENTER_CS", 15, 0));
	FIELD("SYSENTER_ESP", "addr32_t", BITS("%SYSENTER_ESP", 31, 0));
	FIELD("SYSENTER_EIP", "addr32_t", BITS("%SYSENTER_EIP", 31, 0));

	CLOSE_SCOPE(); // sysenter

	REG64("%DebugCtl", 0x1d9);
	FIELD("LBR", "enabledisable_t", BITS("%DebugCtl", 0));
	FIELD("BTF", "enabledisable_t", BITS("%DebugCtl", 1));

	FIELD("LastBranchFromIP", "addr64_t", REG64(0x1db));
	FIELD("LastBranchToIP", "addr64_t", REG64(0x1dc));
	FIELD("LastExceptionFromIP", "addr64_t", REG64(0x1dd));
	FIELD("LastExceptionToIP", "addr64_t", REG64(0x1de));

	REG64("%PAT", 0x277);
	FIELD("PA0", "pat_type_t", BITS("%PAT", 2, 0));
	FIELD("PA1", "pat_type_t", BITS("%PAT", 10, 8));
	FIELD("PA2", "pat_type_t", BITS("%PAT", 18, 16));
	FIELD("PA3", "pat_type_t", BITS("%PAT", 26, 24));
	FIELD("PA4", "pat_type_t", BITS("%PAT", 34, 32));
	FIELD("PA5", "pat_type_t", BITS("%PAT", 42, 40));
	FIELD("PA6", "pat_type_t", BITS("%PAT", 50, 48));
	FIELD("PA7", "pat_type_t", BITS("%PAT", 58, 56));

	//FIXME: more MCG_CAP, MCG_STAT, MCG_CTL, MC*
}
