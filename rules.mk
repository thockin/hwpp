# load optional user-provided saved flags first
BUILD_CONFIG=$(TOPDIR)/BUILD_CONFIG
ifeq ($(BUILD_CONFIG), $(wildcard $(BUILD_CONFIG)))
  ifeq ($(DID_LOAD_BUILD_CONFIG),)
    $(info loading BUILD_CONFIG)
  endif
  include $(BUILD_CONFIG)
endif
export DID_LOAD_BUILD_CONFIG := 1


# project version
PP_VERSION = 0.2.0


# build options
# use '?=' variable assignment so ENV variables can be used.
# Otherwise 'DEBUG= make all' would still build with debugging enabled.

export DEBUG ?= 1	# boolean: 0=no, 1=yes
export STATIC ?= 0	# option: 0=no, 1=yes, 2=partial
export PROFILE ?= 0	# boolean: 0=no, 1=yes


# user-overrideable variables

# CPPFLAGS	# extra pre-processor flags
# CFLAGS	# extra C compiler flags
# CXXFLAGS	# extra C++ compiler flags
# ARCHFLAGS	# output architecture flags
# LDFLAGS	# extra linker flags
# INCLUDES	# extra includes
# LIBS		# extra linker libs, linked statically for non-zero STATIC=
# LIBS_DYN	# extra linker libs, linked dynamically unless STATIC=1
# CWARNS	# extra C compiler warnings
# CXXWARNS	# extra C++ compiler warnings
# DEFS		# extra pre-processor definitions


# per-target variables

# $@_CPPFLAGS	# extra pre-processor flags
# $@_CFLAGS	# extra C compiler flags
# $@_CXXFLAGS	# extra C++ compiler flags
# $@_LDFLAGS	# extra linker flags
# $@_INCLUDES	# extra includes
# $@_LIBS	# extra linker libs, linked statically for non-zero STATIC=
# $@_LIBS_DYN	# extra linker libs, linked dynamically unless STATIC=1
# $@_CWARNS	# extra C compiler warnings
# $@_CXXWARNS	# extra C++ compiler warnings
# $@_DEFS	# extra pre-processor definitions


# build tools

CPP = $(CROSS_COMPILE)cpp
CC  = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++


# build flags

PP_CPPFLAGS = $($@_CPPFLAGS)
PP_CFLAGS = $(ARCHFLAGS) $($@_CFLAGS)
PP_CXXFLAGS = $(PP_CFLAGS) $($@_CXXFLAGS)
PP_LDFLAGS = $(ARCHFLAGS) $($@_LDFLAGS)
PP_CWARNS = -Wall -Wextra -Werror $(CWARNS) $($@_CWARNS)
PP_CXXWARNS = $(PP_CWARNS) -Woverloaded-virtual $(CXXWARNS) $($@_CXXWARNS)
PP_DEFS = -DPP_VERSION="\"$(PP_VERSION)\"" -D_GNU_SOURCE $(DEFS) $($@_DEFS)
PP_INCLUDES = -I$(TOPDIR) $(DIR_INCLUDES) $(INCLUDES) $($@_INCLUDES)
PP_LDLIBS = -lgmpxx -lgmp $(LIBS) $($@_LDLIBS)
PP_LDLIBS_DYN = $(LIBS_DYN) -lstdc++ $($@_LDLIBS_DYN)

ifeq ($(strip $(STATIC)),1)
PP_STATIC = -static
PP_DYNAMIC =
endif
ifeq ($(strip $(STATIC)),2)
PP_STATIC = -Wl,-Bstatic
PP_DYNAMIC = -Wl,-Bdynamic
endif

ifeq ($(strip $(PROFILE)),1)
ifeq ($(strip $(DEBUG)),1)
$(warning WARNING: PROFILE and DEBUG are both enabled)
endif
PP_CFLAGS += -pg
PP_LDFLAGS += -pg
PP_LDLIBS += -lgcov
endif

# debug options should go last
ifeq ($(strip $(DEBUG)),1)
PP_TRACE = $(foreach trace, $(TRACE), -DTRACE_$(trace)=1)
PP_CDEBUG = -O0 -ggdb -DDEBUG -UNDEBUG $(PP_TRACE)
PP_CXXDEBUG = $(PP_CDEBUG) -fno-default-inline
else
PP_CDEBUG = -O2 -DNDEBUG
endif

CPPFLAGS += $(PP_DEFS) $(PP_INCLUDES)
CFLAGS += $(PP_CFLAGS) $(PP_CWARNS) $(PP_CDEBUG)
CXXFLAGS += $(PP_CXXFLAGS) $(PP_CXXWARNS) $(PP_CXXDEBUG)
LDFLAGS += $(PP_LDFLAGS)
LDLIBS += $(PP_STATIC) $(PP_LDLIBS) $(PP_DYNAMIC) $(PP_LDLIBS_DYN)

MAKEFLAGS += --no-print-directory


# common rules

# this is the default rule, which the calling Makefile should define
.PHONY: all
all: make_flags

.PHONY: make_flags
make_flags:
	@\
	NEW=$$($$(which echo) -e \
	       "CC='$$(which $(CC))'\n" \
	       "CXX='$$(which $(CXX))'\n" \
	       "CROSS_COMPILE='$(CROSS_COMPILE)'\n" \
	       "CFLAGS='$(CFLAGS)'\n" \
	       "CXXFLAGS='$(CXXFLAGS)'\n" \
	       "CPPFLAGS='$(CPPFLAGS)'\n" \
	       "LDFLAGS='$(LDFLAGS)'\n" \
	       "LDLIBS='$(LDLIBS)'\n"); \
	OLD=$$(cat .make_flags 2>/dev/null); \
	if [ "$$NEW" != "$$OLD" ]; then \
		$(MAKE) clean; \
		echo "$$NEW" > .make_flags; \
	fi

.PHONY: clean_make_flags
clean_make_flags:
	@$(RM) .make_flags

.PHONY: clean
clean: clean_make_flags

.PHONY: dep depend
dep depend:
	@$(RM) .depend
	@$(MAKE) .depend

.depend:
	@for f in $^; do \
		OBJ=$$(echo $$f | sed 's/\.cp*$$/.o/'); \
		$(CPP) $(CPPFLAGS) -MM $$f -MT $$OBJ; \
	done > $@.tmp; \
	diff -w -B -q $@ $@.tmp >/dev/null 2>&1; \
	if [ $$? != 0 ]; then \
		mv -f $@.tmp $@; \
	else \
		$(RM) $@.tmp; \
	fi

# a generic empty target to force some rules
.PHONY: FORCE
FORCE:

.PHONY: run_tests
run_tests:
	@for f in $(RUN_TESTS); do \
		echo -n "TEST $$f: "; \
		./$$f > $$f.err 2>&1; \
		if [ "$$?" -eq "0" ]; then \
			echo PASS; \
		else \
			echo FAIL; \
		fi; \
		cat $$f.err | sed 's/^/  /'; \
		$(RM) -f $$f.err; \
	done 2>/dev/null

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
