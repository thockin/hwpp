/*
 * This is a temporary home for calls to global stuff that should actually
 * be defined in the PP language.
 */
#ifndef PP_DEVICE_INIT_H__
#define PP_DEVICE_INIT_H__

class pp_scope;

extern void
global_datatypes_init(pp_scope *platform);

extern void
pci_datatypes_init(pp_scope *platform);

#endif // PP_DEVICE_INIT_H__
