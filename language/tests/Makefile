TOPDIR ?= ../..
include $(TOPDIR)/pp.mk

SRCS = 
OBJS = $(SRCS:.cpp=.o)

TESTS = variable_test \
        syntax_tree_test
TESTS_SRCS = $(TESTS:=.cpp)


all: libpptest.a $(TESTS)

../tests/libpptest.a:
	$(MAKE) -C ../tests libpptest.a

$(TESTS): %: %.o libpptest.a

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
