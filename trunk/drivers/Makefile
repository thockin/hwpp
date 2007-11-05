DRIVERS = io mem pci

all test clean dep:
	@for d in $(DRIVERS); do \
		$(MAKE) -C $$d $@; \
	done

lib:
	@for d in $(DRIVERS); do \
		ar rcs $(LIBNAME) $$d/$${d}_driver_module.o; \
	done
