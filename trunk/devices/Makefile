TOPDIR = ..
include $(TOPDIR)/rules.mk

SRCS = global.cpp \
	pci/pci.cpp \
	pci/generic_device.cpp \
	cpuid/cpuid.cpp \
	cpuid/generic_device.cpp
OBJS = $(SRCS:.cpp=.o)


all: all_devices.o

all_devices.o: $(OBJS)
	$(CXX) $(LDFLAGS) -nostdlib -Wl,-r -o $@ $^

.PHONY: test
test:
	@true

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a .depend

.depend: $(SRCS)
