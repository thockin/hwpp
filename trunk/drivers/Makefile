DRIVERS = io pci

all:
	for d in $(DRIVERS); do \
		$(MAKE) -C $$d all; \
	done

lib:
	for d in $(DRIVERS); do \
		ar rcs $(LIBNAME) $$d/$${d}_driver_module.o; \
	done

test:
	for d in $(DRIVERS); do \
		$(MAKE) -C $$d test; \
	done

clean:
	for d in $(DRIVERS); do \
		$(MAKE) -C $$d clean; \
	done
