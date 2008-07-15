#ifndef PP_DEVICES_MSR_GENERIC_DEVICE_H__
#define PP_DEVICES_MSR_GENERIC_DEVICE_H__

extern void
MSR_SCOPE(const string &name, const pp_value &cpu);

extern void
MSR_REGSCOPE(const string &name, const pp_value &address);

extern void
msr_generic_device();

#endif // PP_DEVICES_MSR_GENERIC_DEVICE_H__
