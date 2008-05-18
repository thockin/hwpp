#ifndef PP_DEVICES_CPUID_GENERIC_DEVICE_H__
#define PP_DEVICES_CPUID_GENERIC_DEVICE_H__

extern void
cpuid_generic_device(const pp_value &cpu);

extern void
CPUID_SCOPE(const string &name, const pp_value &cpu, const pp_value &func);

#endif // PP_DEVICES_CPUID_GENERIC_DEVICE_H__
