TOPDIR = ..
include $(TOPDIR)/rules.mk

SRCS = global.cpp \
	pci/pci.cpp \
	pci/generic_device.cpp
OBJS = $(SRCS:.cpp=.o)


all: all_devices.o

all_devices.o: $(OBJS)
	$(LD) -r -o $@ $^

.PHONY: test
test:

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend $(OBJS)

.depend: $(SRCS)
