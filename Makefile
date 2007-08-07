INCLUDES =
CXXFLAGS = -Wall -Werror $(INCLUDES) -g -DBUILD_TESTS

libpp_SRCS = utils.cpp magic_regs.cpp
libpp_OBJS = $(libpp_SRCS:.cpp=.o)

TEST_DIRS = tests drivers

all: libpp.a

libpp.a: .depend $(libpp_OBJS)
	ar rcs $@ $^

.PHONY: test
test:
	@for d in $(TEST_DIRS); do \
		$(MAKE) -C $$d test; \
	done

.PHONY: clean
clean:
	$(RM) *.o *.a
	@for d in $(TEST_DIRS); do \
		$(MAKE) -C $$d clean; \
	done

.PHONY: dep
dep .depend: $(SRCS)
	@for f in $(SRCS); do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
