#ifndef PP_DEVICES_PCI_GENERIC_DEVICE_H__
#define PP_DEVICES_PCI_GENERIC_DEVICE_H__

namespace pp {
namespace device {

extern void
PCI_SCOPE(const string &name, const Value &seg, const Value &bus,
    const Value &dev, const Value &func);

}  // namespace device
}  // namespace pp

#endif // PP_DEVICES_PCI_GENERIC_DEVICE_H__
