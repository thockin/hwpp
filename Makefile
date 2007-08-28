TOPDIR = $(shell pwd)
INCLUDES = -I$(TOPDIR)
CXXFLAGS = -Wall -Werror $(INCLUDES) -g

libpp_SRCS = utils.cpp \
	magic_regs.cpp \
	drivers.cpp
libpp_OBJS = $(libpp_SRCS:.cpp=.o) devices/all_devices.o


all: libpp.a

libpp.a: .depend $(libpp_OBJS) drivers devices
	ar rcs $@ $(libpp_OBJS)
	$(MAKE) -C drivers lib LIBNAME=$(TOPDIR)/$@

.PHONY: drivers
drivers:
	$(MAKE) -C drivers

.PHONY: devices
devices:
	$(MAKE) -C devices

.PHONY: test
test:
	$(MAKE) -C tests test
	$(MAKE) -C drivers test

.PHONY: clean
clean:
	$(RM) *.o *.a
	$(MAKE) -C tests clean
	$(MAKE) -C drivers clean

.PHONY: dep
dep .depend: $(libpp_SRCS)
	@for f in $(libpp_SRCS); do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
