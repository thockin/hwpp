TOPDIR = $(shell pwd)
include rules.mk

SRCS = utils.cpp \
	magic_regs.cpp \
	drivers.cpp \
	pp_path.cpp \
	pp_scope.cpp
OBJS = $(SRCS:.cpp=.o)


all: libpp.a drivers


libpp.a: $(OBJS) devices/all_devices.o
	ar rcs $@ $(OBJS) devices/all_devices.o

.PHONY: drivers devices
drivers devices:
	@$(MAKE) -C $@

.PHONY: examples
examples: all
	@$(MAKE) -C $@

# This target is a hack until we have a real language.  It's written this
# way on purpose, to work around make being too smart for it's own good.
devices/all_devices.o: FORCE
	@$(MAKE) devices

.PHONY: test
test: all
	@$(MAKE) -C tests test
	@$(MAKE) -C drivers test
	@$(MAKE) -C devices test

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a .depend
	@$(MAKE) -C tests clean
	@$(MAKE) -C drivers clean
	@$(MAKE) -C devices clean
	@$(MAKE) -C examples clean

.depend: $(SRCS)
