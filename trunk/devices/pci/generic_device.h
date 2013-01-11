#ifndef HWPP_DEVICES_PCI_GENERIC_DEVICE_H__
#define HWPP_DEVICES_PCI_GENERIC_DEVICE_H__

namespace hwpp {
namespace device {

extern void
PCI_SCOPE(const string &name, const Value &seg, const Value &bus,
    const Value &dev, const Value &func);

}  // namespace device
}  // namespace hwpp

#endif // HWPP_DEVICES_PCI_GENERIC_DEVICE_H__
