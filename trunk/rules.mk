# load optional user-provided saved flags first
BUILD_CONFIG=$(TOPDIR)/BUILD_CONFIG
ifeq ($(BUILD_CONFIG), $(wildcard $(BUILD_CONFIG)))
  ifeq ($(DID_LOAD_BUILD_CONFIG),)
    $(info loading $(BUILD_CONFIG))
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
# CXXFLAGS	# extra compiler flags
# ARCHFLAGS	# output architecture flags
# LDFLAGS	# extra linker flags
# INCLUDES	# extra includes
# LIBS		# extra linker libs, linked statically for non-zero STATIC=
# LIBS_DYN	# extra linker libs, linked dynamically unless STATIC=1
# WARNS		# extra compiler warnings
# DEFS		# extra pre-processor definitions


# per-target variables

# $@_CPPFLAGS	# extra pre-processor flags
# $@_CXXFLAGS	# extra compiler flags
# $@_LDFLAGS	# extra linker flags
# $@_INCLUDES	# extra includes
# $@_LIBS	# extra linker libs, linked statically for non-zero STATIC=
# $@_LIBS_DYN	# extra linker libs, linked dynamically unless STATIC=1
# $@_WARNS	# extra compiler warnings
# $@_DEFS	# extra pre-processor definitions


# build tools

CXX = $(CROSS_COMPILE)g++
CC = $(CXX)
CPP = $(CROSS_COMPILE)cpp


# build flags

PP_CPPFLAGS = $($@_CPPFLAGS)
PP_CXXFLAGS = $(ARCHFLAGS) $($@_CXXFLAGS)
PP_LDFLAGS = $(ARCHFLAGS) $($@_LDFLAGS)
PP_WARNS = -Wall -Wextra -Werror -Woverloaded-virtual $(WARNS) $($@_WARNS)
PP_DEFS = -DPP_VERSION="\"$(PP_VERSION)\"" -D_GNU_SOURCE $(DEFS) $($@_DEFS)
PP_INCLUDES = -I$(TOPDIR) $(DIR_INCLUDES) $(INCLUDES) $($@_INCLUDES)
PP_LDLIBS = -lgmpxx -lgmp $(LIBS) $($@_LDLIBS)
PP_LDLIBS_DYN = $(LIBS_DYN) $($@_LDLIBS_DYN)

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
PP_CXXFLAGS += -pg
PP_LDFLAGS += -pg
PP_LDLIBS += -lgcov
endif

# debug options should go last
ifeq ($(strip $(DEBUG)),1)
PP_TRACE = $(foreach trace, $(TRACE), -DTRACE_$(trace)=1)
PP_DEBUG = -O0 -ggdb -DDEBUG -UNDEBUG -fno-default-inline $(PP_TRACE)
else
PP_DEBUG = -O2 -DNDEBUG
endif

CPPFLAGS += $(PP_DEFS) $(PP_INCLUDES)
CXXFLAGS += $(PP_CXXFLAGS) $(PP_WARNS) $(PP_DEBUG)
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
	       "CXX='$$(which $(CXX))'\n" \
	       "CROSS_COMPILE='$(CROSS_COMPILE)'\n" \
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
	done > $@

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
		cat $$f.err | while read LINE; do \
			echo "    $$LINE"; \
		done; \
		$(RM) -f $$f.err; \
	done

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
