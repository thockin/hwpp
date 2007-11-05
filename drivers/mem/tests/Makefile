TOPDIR = ../../..
include $(TOPDIR)/rules.mk

LIBS += -L$(TOPDIR)/tests -lpptest -L$(TOPDIR) -lpp
INCLUDES += -I.. -I$(TOPDIR)/tests

TESTS = mem_binding_test
TESTS_SRCS = $(TESTS:=.cpp)


all: $(TESTS)

$(TESTS): %: %.o

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
