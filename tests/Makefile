TOPDIR ?= ..
include $(TOPDIR)/pp.mk

SRCS = test_helpers.cpp
OBJS = $(SRCS:.cpp=.o)

TESTS = keyed_vector_test \
	filesystem_test \
	bit_buffer_test \
	bignum_test \
	regex_test \
	printfxx_test \
	syserror_test \
	sockets_test \
	path_test \
	dirent_test \
	binding_test \
	register_test \
	regbits_test \
	datatype_test \
	field_test \
	scope_test \
	array_test \
	alias_test \
	fake_language_test \
	magic_regs_test
TESTS_SRCS = $(TESTS:=.cpp)


all: libpptest.a $(TESTS)

libpptest.a: $(OBJS)
	ar rcs $@ $^
	ranlib $@

$(TESTS): %: %.o $(TOPDIR)/libpp.a libpptest.a

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
