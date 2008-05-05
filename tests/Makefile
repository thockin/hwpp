TOPDIR = ..
include $(TOPDIR)/rules.mk

SRCS = test_helpers.cpp
OBJS = $(SRCS:.cpp=.o)

TESTS = keyed_vector_test \
	filesystem_test \
	bitbuffer_test \
	bignum_test \
	regex_test \
	pp_path_test \
	pp_dirent_test \
	pp_binding_test \
	pp_register_test \
	pp_regbits_test \
	pp_datatype_test \
	pp_field_test \
	pp_scope_test \
	pp_array_test \
	fake_language_test \
	magic_regs_test
TESTS_SRCS = $(TESTS:=.cpp)


all: libpptest.a $(TESTS)

libpptest.a: $(OBJS)
	ar rcs $@ $^

$(TESTS): %: %.o $(TOPDIR)/libpp.a libpptest.a

.PHONY: test
test: all
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a .depend $(TESTS)

.depend: $(SRCS) $(TESTS_SRCS)
