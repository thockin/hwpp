TOPDIR = $(shell pwd)
include pp.mk

DRIVER_LIB = drivers/libdrivers.a

SRCS = fake_language.cpp \
	runtime.cpp \
	language.cpp \
	magic_regs.cpp \
	drivers.cpp \
	pp_path.cpp \
	pp_scope.cpp \
	version.cpp
OBJS = $(SRCS:.cpp=.o)


all: libpp.a

# add a driver in the DRIVER_LIB to another archive
define add_driver
	ar -x $(DRIVER_LIB) $(1);
	ar rcs $(2) $(1);
	$(RM) $(1);
endef

libpp.a: $(OBJS) devices/all_devices.o $(DRIVER_LIB)
	ar rcs $@ $(OBJS) devices/all_devices.o
	@$(foreach drvobj,$(shell ar -t $(DRIVER_LIB)),$(call add_driver,$(drvobj),$@))
	ranlib $@

$(DRIVER_LIB): drivers
	@$(MAKE) -C drivers $$(basename $@)

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
	@$(RM) $(OBJS) *.o *.a
	@$(MAKE) -C tests clean
	@$(MAKE) -C drivers clean
	@$(MAKE) -C devices clean
	@$(MAKE) -C examples clean

.PHONY: distclean
distclean:
	@$(RM) $(BUILD_CONFIG)
	@$(MAKE) -C tests distclean
	@$(MAKE) -C drivers distclean
	@$(MAKE) -C devices distclean
	@$(MAKE) -C examples distclean

.depend: $(SRCS)
