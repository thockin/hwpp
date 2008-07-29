TOPDIR = ..
include $(TOPDIR)/rules.mk

DRIVERS = io mem pci msr cpuid
DRIVER_LIB = libdrivers.a
SRCS = all_drivers.cpp
OBJS = $(SRCS:.cpp=.o)

DRIVER_MOD_SFX = _driver_module.o


all: $(OBJS)
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@ || exit 1; \
	done

DRIVERS_TO_ADD=$(foreach dir,$(DRIVERS), $(wildcard $(dir)/*$(DRIVER_MOD_SFX)))

DRIVER_INIT_FUNCS = $(foreach dir,$(DRIVERS), \
	load_$(dir)_driver, \
)
DRIVER_INIT_PROTOS = $(foreach dir,$(DRIVERS), \
	extern pp_driver *load_$(dir)_driver(); \
)

lib: $(DRIVER_LIB)

$(DRIVER_LIB): $(OBJS) $(DRIVERS_TO_ADD)
	ar rcs $@ $^
	ranlib $@

all_drivers.o ... : DEFS += -DDRIVER_INIT_FUNCS="$(DRIVER_INIT_FUNCS)" \
		-DDRIVER_INIT_PROTOS="$(DRIVER_INIT_PROTOS)"

test:
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@ || exit 1; \
	done

.PHONY: clean
clean:
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@; \
	done
	@$(RM) $(DRIVER_LIB) $(OBJS) .depend
