INCLUDES = -I../.. -I../../test
CXXFLAGS = -Wall -Werror $(INCLUDES) -g

lib_SRCS = pci_driver.cpp pci_device.cpp
lib_OBJS = $(lib_SRCS:.cpp=.o)

all: pci.a

pci.a: .depend $(lib_OBJS)
	ar rcs $@ $^

.PHONY: test
test:
	$(MAKE) -C tests test

.PHONY: clean
clean:
	$(RM) *.o *.a
	$(MAKE) -C tests clean

.PHONY: dep
dep .depend: $(SRCS)
	@for f in $(SRCS); do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
