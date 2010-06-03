/*
 * This is a temporary home for calls to global stuff that should actually
 * be defined in the PP language.
 */
#ifndef PP_DEVICE_INIT_H__
#define PP_DEVICE_INIT_H__

#include "runtime.h"
#include "scope.h"

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


// apps should call this to bootstrap themselves
static inline ScopePtr
initialize_device_tree() {
	ScopePtr root = global_runtime()->current_context()->scope();
	device::global_datatypes_init();
	device::pci_datatypes_init();
	device::cpuid_datatypes_init();
	device::msr_datatypes_init();
	return root;
}

}  // namespace pp

#endif // PP_DEVICE_INIT_H__
