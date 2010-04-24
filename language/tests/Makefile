TOPDIR ?= ../..
include $(TOPDIR)/pp.mk

SRCS = 
OBJS = $(SRCS:.cpp=.o)

TESTS = variable_test \
        syntax_tree_test
TESTS_SRCS = $(TESTS:=.cpp)

all: $(TESTS)

variable_test: ../variable.o

$(TESTS): %: %.o

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
