#ifndef PP_DEVICES_PCI_GENERIC_DEVICE_H__
#define PP_DEVICES_PCI_GENERIC_DEVICE_H__

extern void
pci_generic_device();

extern void
PCI_SCOPE(const string &name, const pp_value &seg, const pp_value &bus,
    const pp_value &dev, const pp_value &func);

#endif // PP_DEVICES_PCI_GENERIC_DEVICE_H__
