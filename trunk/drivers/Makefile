DRIVERS = io mem pci msr
DRIVER_LIB = libdrivers.a

DRIVER_MOD_SFX = _driver_module.o

all test dep:
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@ || exit 1; \
	done

DRIVERS_TO_ADD=$(foreach dir,$(DRIVERS), $(wildcard $(dir)/*$(DRIVER_MOD_SFX)))

$(DRIVER_LIB): $(DRIVERS_TO_ADD)
	ar rcs $@ $^

lib: $(DRIVER_LIB)

.PHONY: clean
clean:
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@; \
	done
	@$(RM) $(DRIVER_LIB)
