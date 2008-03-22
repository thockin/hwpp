TOPDIR = ../../..
include $(TOPDIR)/rules.mk

INCLUDES += -I.. -I$(TOPDIR)/tests

TESTS = io_io_test
TESTS_SRCS = $(TESTS:=.cpp)


all: $(TESTS)

$(TESTS): %: %.o ../io.so $(TOPDIR)/libpp.a $(TOPDIR)/tests/libpptest.a

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a .depend $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
