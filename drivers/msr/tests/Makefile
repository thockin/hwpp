TOPDIR = ../../..
include $(TOPDIR)/rules.mk

INCLUDES += -I.. -I$(TOPDIR)/tests

TESTS = msr_io_test
TESTS_SRCS = $(TESTS:=.cpp)


all: $(TESTS)

$(TESTS): %: %.o ../msr_driver_module.o \
		$(TOPDIR)/libpp.a $(TOPDIR)/tests/libpptest.a

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a .depend $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
