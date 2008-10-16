TOPDIR ?= ..
include $(TOPDIR)/pp.mk

SRCS = global.cpp \
	pci/pci.cpp \
	pci/generic_device.cpp \
	cpu/cpu.cpp \
	cpuid/cpuid.cpp \
	cpuid/generic_device.cpp \
	msr/msr.cpp \
	msr/generic_device.cpp \
	amd/k8.cpp
OBJS = $(SRCS:.cpp=.o)


all: all_devices.o

all_devices.o: $(OBJS)
	$(CXX) $(LDFLAGS) -nostdlib -Wl,-r -o $@ $^

.PHONY: test
test:
	@true

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a

.depend: $(SRCS)
