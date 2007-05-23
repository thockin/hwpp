INCLUDES =
CXXFLAGS = -Wall -Werror $(INCLUDES) -g -DBUILD_TESTS

all: .depend

test:
	$(MAKE) -C tests test

clean:
	$(MAKE) -C tests clean
	$(RM) *.o 

dep .depend:
	@for f in $(SRCS); do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
