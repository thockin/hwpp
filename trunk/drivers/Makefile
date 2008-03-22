TOPDIR = ..
include $(TOPDIR)/rules.mk

DRIVERS = io mem pci msr cpuid

all test:
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@ || exit 1; \
	done

.PHONY: clean
clean:
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@; \
	done
	@$(RM) .depend
