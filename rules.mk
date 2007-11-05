# build options

DEBUG = 1
STATIC = 1

# build tools

CXX = g++
CC = $(CXX)

# build flags

PP_CXXFLAGS = -O2 $(INCLUDES)
PP_WARNS = -Wall -Werror $(WARNS)
PP_DEFS = $(DEFS)
PP_INCLUDES = -I$(TOPDIR) $(INCLUDES)
ifneq ($(strip $(DEBUG)),)
PP_DEBUG = -O0 -ggdb -DDEBUG
endif

PP_LIBS = $(LIBS)
ifneq ($(strip $(STATIC)),)
PP_STATIC = -static
endif

CXXFLAGS += $(PP_CXXFLAGS) $(PP_WARNS) $(PP_DEFS) $(PP_INCLUDES) $(PP_DEBUG)
LDLIBS += $(PP_LIBS) $(PP_STATIC)

MAKEFLAGS = --no-print-directory

# common rules

# this is the default rule, which the calling Makefile should define
all:

.PHONY: dep
dep depend:
	@$(RM) .depend
	@$(MAKE) .depend

.depend:
	@for f in $^; do \
		OBJ=$$(echo $$f | sed 's/\.cp*$$/.o/'); \
		$(CPP) $(PP_INCLUDES) -MM $$f -MT $$OBJ; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
# if dependency info exists, read it in
include .depend
else
# otherwise force make to reload this Makefile
Makefile: .depend
	@touch $@
endif

# a generic empty target to force some rules
FORCE:

run_tests:
	@for f in $(RUN_TESTS); do \
		echo -n "Running $$f: "; \
		./$$f > $$f.err 2>&1; \
		if [ "$$?" -eq "0" ]; then \
			echo PASS; \
		else \
			echo FAIL; \
			cat $$f.err | while read LINE; do \
				echo -e "    $$LINE"; \
			done; \
		fi; \
		$(RM) -f $$f.err; \
	done

