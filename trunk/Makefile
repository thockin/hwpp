TOPDIR = $(shell pwd)
include rules.mk

SRCS = utils.cpp \
	magic_regs.cpp \
	drivers.cpp
OBJS = $(SRCS:.cpp=.o)


all: libpp.a

libpp.a: $(OBJS) devices/all_devices.o
	ar rcs $@ $^
	$(MAKE) -C drivers all lib LIBNAME=$(TOPDIR)/$@ || $(RM) $@

devices/all_devices.o:
	@$(MAKE) -C devices

.PHONY: drivers devices examples
drivers devices examples:
	@$(MAKE) -C $@

.PHONY: test
test:
	@$(MAKE) -C tests test
	@$(MAKE) -C drivers test
	@$(MAKE) -C devices test

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend
	@$(MAKE) -C tests clean
	@$(MAKE) -C drivers clean
	@$(MAKE) -C devices clean
	@$(MAKE) -C examples clean

.depend: $(SRCS)
