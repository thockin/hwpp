INCLUDES = -I..
CXXFLAGS = -Wall -Werror $(INCLUDES) -g -DBUILD_TESTS
LDFLAGS = -L. -lpptest -L.. -lpp

TESTS = keyed_vector_test \
	filesystem_test \
	pp_datatype_test \
	pp_container_test \
	pp_binding_test \
	pp_field_test \
	pp_register_test \
	pp_scope_test \
	pp_space_test \
	pp_device_test \
	pp_dirent_test \
	pp_path_test \
	utils_test
TESTS_SRCS = $(TESTS:=.cpp)

libpptest_SRCS = test_helpers.cpp
libpptest_OBJS = $(libpptest_SRCS:.cpp=.o)


all: libpptest.a $(TESTS)

libpptest.a: .depend $(libpptest_OBJS)
	ar rcs $@ $^

test: .depend $(TESTS)
	@for f in $(TESTS); do \
		echo -n "Running $$f: "; \
		./$$f; \
		if [ "$$?" -eq "0" ]; then echo PASS; else echo FAIL; fi; \
	done

$(TESTS): %: %.o
	$(CXX) -o $@ $^ $(LDFLAGS)

clean:
	$(RM) *.o *.a $(TESTS)

.PHONY: dep
dep .depend: $(TESTS_SRCS)
	@for f in $^; do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
