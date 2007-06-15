TESTS = keyed_vector_test \
	pp_datatype_test \
	pp_container_test \
	pp_binding_test \
	pp_field_test \
	pp_register_test \
	pp_scope_test \
	pp_space_test \
	pp_device_test \
	pp_path_test
SRCS = $(TESTS:=.cpp)

INCLUDES = -I..
CXXFLAGS = -Wall -Werror $(INCLUDES) -g -DBUILD_TESTS

all: .depend $(TESTS)

test: .depend $(TESTS)
	@for f in $(TESTS); do \
		echo -n "Running $$f: "; \
		./$$f; \
		if [ "$$?" -eq "0" ]; then echo PASS; else echo FAIL; fi; \
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
