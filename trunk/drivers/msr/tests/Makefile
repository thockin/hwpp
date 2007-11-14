TOPDIR = ../../..
include $(TOPDIR)/rules.mk

INCLUDES += -I.. -I$(TOPDIR)/tests

TESTS = msr_binding_test
TESTS_SRCS = $(TESTS:=.cpp)


all: $(TESTS)

$(TESTS): %: %.o $(TOPDIR)/libpp.a $(TOPDIR)/tests/libpptest.a

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
