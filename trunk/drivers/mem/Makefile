INCLUDES = -I../.. -I../../test
CXXFLAGS = -Wall -Werror $(INCLUDES) -g

SRCS = mem_driver.cpp
OBJS = $(SRCS:.cpp=.o)

all: mem_driver_module.o

mem_driver_module.o: .depend $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: test
test:
	$(MAKE) -C tests test

.PHONY: clean
clean:
	$(RM) *.o
	$(MAKE) -C tests clean

.PHONY: dep
dep .depend: $(SRCS)
	@for f in $(SRCS); do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
