DRIVERS = pci

all:
	for d in $(DRIVERS); do \
		$(MAKE) -C $$d all; \
	done

test:
	for d in $(DRIVERS); do \
		$(MAKE) -C $$d test; \
	done

clean:
	for d in $(DRIVERS); do \
		$(MAKE) -C $$d test; \
	done
