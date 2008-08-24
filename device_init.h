/*
 * This is a temporary home for calls to global stuff that should actually
 * be defined in the PP language.
 */
#ifndef PP_DEVICE_INIT_H__
#define PP_DEVICE_INIT_H__

extern void
global_datatypes_init();
extern void
pci_datatypes_init();
extern void
cpuid_datatypes_init();
extern void
msr_datatypes_init();

#endif // PP_DEVICE_INIT_H__
