/*
 * This is a temporary home for calls to global stuff that should actually
 * be defined in the PP language.
 */
#ifndef PP_DEVICE_INIT_H__
#define PP_DEVICE_INIT_H__

namespace pp {
namespace device {

void
global_datatypes_init();

void
pci_datatypes_init();

void
cpuid_datatypes_init();

void
msr_datatypes_init();

}  // namespace device
}  // namespace pp

#endif // PP_DEVICE_INIT_H__
