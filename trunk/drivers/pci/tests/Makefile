TESTS = pci_binding_test
SRCS = $(TESTS:=.cpp)

INCLUDES = -I.. -I../../.. -I ../../../tests
CXXFLAGS = -Wall -Werror $(INCLUDES) -g

all: .depend $(TESTS)

test: .depend $(TESTS)
	@for f in $(TESTS); do \
		echo -ne "Running $$f: "; \
		./$$f; \
		if [ $$? == 0 ]; then echo PASS; else echo FAIL; fi; \
	done

$(TESTS): %: %.o
	$(CXX) -o $@ $^

clean:
	$(RM) *.o $(TESTS)

.PHONY: dep
dep .depend: $(SRCS)
	@for f in $(SRCS); do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
