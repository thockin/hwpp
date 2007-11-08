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

# NOTE: 'sinclude' is "silent-include".  This suppresses a warning if
# .depend does not exist.  Since Makefile includes this file, and this
# file includes .depend, .depend is itself "a makefile" and Makefile is
# dependent on it.  Any makefile for which there is a rule (as above for
# .depend) will be evaluated before anything else.  If the rule executes
# and the makefile is updated, make will reload the original Makefile and
# start over.
#
# This means that the .depend rule will always be checked first.  If
# .depend gets rebuilt, then the dependencies we have already sincluded
# must have been stale.  Make starts over, the old dependencies are
# tossed, and the new dependencies are sincluded.
#
# So why use 'sinclude' instead of 'include'?  We want to ALWAYS make
# Makefile depend on .depend, even if .depend doesn't exist yet.  But we
# don't want that pesky warning.
sinclude .depend

# a generic empty target to force some rules
FORCE:

run_tests:
	@for f in $(RUN_TESTS); do \
		echo -n "TEST $$f: "; \
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
