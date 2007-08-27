TOPDIR = $(shell pwd)
INCLUDES = -I$(TOPDIR)
CXXFLAGS = -Wall -Werror $(INCLUDES) -g

libpp_SRCS = utils.cpp \
	magic_regs.cpp \
	drivers.cpp \
	devices/global.cpp \
	devices/pci/pci.cpp
libpp_OBJS = $(libpp_SRCS:.cpp=.o)


all: libpp.a

libpp.a: .depend $(libpp_OBJS) drivers
	ar rcs $@ $(libpp_OBJS)
	$(MAKE) -C drivers lib LIBNAME=$(TOPDIR)/$@

.PHONY: drivers
drivers:
	$(MAKE) -C drivers

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
