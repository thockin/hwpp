# This file is intended to be generically useful make infrastructure.
# Anything that is really project specific should be defined outside of
# this file.

# The top-level Makefile must define TOPDIR.
ifeq ($(strip $(TOPDIR)),)
  $(error TOPDIR is not defined)
endif
export TOPDIR

# Load optional user-provided saved flags first.
ifneq ($(strip $(BUILD_CONFIG)),)
  ifeq ($(BUILD_CONFIG), $(wildcard $(BUILD_CONFIG)))
    ifeq ($(DID_LOAD_BUILD_CONFIG),)
      $(info loading BUILD_CONFIG)
    endif
    include $(BUILD_CONFIG)
  endif
  export DID_LOAD_BUILD_CONFIG := 1
endif


# Common build options
#
# The caller should define default values for these before including this
# file.  Use '?=' variable assignment so ENV variables can be used.
# Otherwise 'DEBUG=1 make all' would still build with debugging disabled.

export DEBUG ?= 0       # boolean: 0=no, 1=yes
export STATIC ?= 0      # option:  0=no, 1=yes, 2=partial
export PROFILE ?= 0     # boolean: 0=no, 1=yes


# Overrideable flags variables
#
# CPPFLAGS      # extra pre-processor flags
# CFLAGS        # extra C compiler flags
# CXXFLAGS      # extra C++ compiler flags
# CWARNS        # extra C compiler warnings
# CXXWARNS      # extra C++ compiler warnings
# ARCHFLAGS     # output architecture flags
# INCLUDES      # extra includes
# DEFS          # extra pre-processor definitions
# LDFLAGS       # extra linker flags
# LIBS          # extra linker libs, linked statically for non-zero STATIC=
# LIBS_DYN      # extra linker libs, linked dynamically unless STATIC=1
# TRACE         # a list of trace names (e.g.: TRACE="FOO BAR" turns into
#               #   -DTRACE_FOO -DTRACE_BAR)


# Build tools

CPP    = $(CROSS_COMPILE)cpp
CC     = $(CROSS_COMPILE)gcc
CXX    = $(CROSS_COMPILE)g++
LEX    = flex
YACC   = bison
AR     = ar
RANLIB = ranlib


# Build flags

PRJ_CPPFLAGS   =
PRJ_CFLAGS     = $(ARCHFLAGS)
PRJ_CXXFLAGS   = $(PRJ_CFLAGS)
PRJ_LDFLAGS    = $(ARCHFLAGS)
PRJ_CWARNS     = -W -Wall -Wextra -Werror $(CWARNS)
PRJ_CXXWARNS   = $(PRJ_CWARNS) -Woverloaded-virtual $(CXXWARNS)
PRJ_VER_DEFS   = -DPRJ_VER_MAJOR=$(PRJ_VER_MAJOR) \
                 -DPRJ_VER_MINOR=$(PRJ_VER_MINOR) \
                 -DPRJ_VER_MICRO=$(PRJ_VER_MICRO)
PRJ_DEFS       = $(DEFS) $(PRJ_VER_DEFS)
PRJ_INCLUDES   = -iquote $(TOPDIR)/.vinclude $(INCLUDES)
PRJ_LDLIBS     = $(LIBS)
PRJ_LDLIBS_DYN = $(LIBS_DYN)

ifeq ($(strip $(STATIC)),1)
PRJ_STATIC  = -static
PRJ_DYNAMIC =
endif
ifeq ($(strip $(STATIC)),2)
PRJ_STATIC  = -Wl,-Bstatic
PRJ_DYNAMIC = -Wl,-Bdynamic
endif

ifeq ($(strip $(PROFILE)),1)
ifeq ($(strip $(DEBUG)),1)
$(warning WARNING: PROFILE and DEBUG are both enabled)
endif
PRJ_CFLAGS  += -pg
PRJ_LDFLAGS += -pg
PRJ_LDLIBS  += -lgcov
endif

# Debug options should go last
ifeq ($(strip $(DEBUG)),1)
PRJ_TRACE    = $(foreach trace, $(TRACE), -DTRACE_$(trace)=1)
PRJ_CDEBUG   = -O0 -ggdb -DDEBUG -UNDEBUG $(PRJ_TRACE)
PRJ_CXXDEBUG = $(PRJ_CDEBUG) -fno-default-inline
else
PRJ_CDEBUG   = -O2 -DNDEBUG
endif

CPPFLAGS += $(PRJ_DEFS) $(PRJ_INCLUDES)
CFLAGS   += $(PRJ_CFLAGS) $(PRJ_CWARNS) $(PRJ_CDEBUG)
CXXFLAGS += $(PRJ_CXXFLAGS) $(PRJ_CXXWARNS) $(PRJ_CXXDEBUG)
LDFLAGS  += $(PRJ_LDFLAGS)
LDLIBS   += $(PRJ_STATIC) $(PRJ_LDLIBS) $(PRJ_DYNAMIC) $(PRJ_LDLIBS_DYN)

# This is used in the suffix rules below.  It's defined here simply to avoid
# duplication.  Suffix rules must call this as $(MKDEP) $(SRC_FILE).
MKDEP = mkdep() { \
	    if [ -z "$$1" ]; then \
	        echo "ERROR: mkdep requires 1 argument"; \
	        return 1; \
	    fi; \
	    OBJ=$$(echo "$$1" | sed 's/\.[^.]*$$/.o/'); \
	    DEP=$$(dirname $$OBJ)/.$$(basename $$OBJ).d; \
	    $(CPP) $(CPPFLAGS) -MM -MG "$$1" -MT $$OBJ > $$DEP; \
	    cp -f $$DEP $$DEP.tmp; \
	    cat $$DEP.tmp | sed -e 's/.*://' -e 's/\\$$//' \
	        | fmt -1 \
	        | sed -e 's/^ *//' -e 's/$$/:/' >> $$DEP; \
	    rm -f $$DEP.tmp; \
	}; \
	mkdep

.cpp.o .cc.o:
	$(CXX) -c -o $@ $(CXXFLAGS) $(CPPFLAGS) $<
	@$(MKDEP) $<

.c.o:
	$(CC) -c -o $@ $(CFLAGS) $(CPPFLAGS) $<
	@$(MKDEP) $<


# Common rules

# This is the default rule, which the top-level Makefile should define.
.PHONY: all
all:

# This tracks the core flags that should trigger a rebuild.  It is included
# so that make treats it as "a Makefile".  Any makefile for which there is a
# rule (which there is for this) will be evaluated before anything else.  If
# the rule executes and the makefile is updated, make will reload the
# original Makefile and start over.
#
# If the .previous_make_flags target detects a change in flags, it can
# make clean and update the target file, which will cause make to restart,
# which is exactly what we want.  Consider the case of 'make -j'.  We need
# all of this to happen *before* any other rules, or else the 'make clean'
# can happen non-deterministically wrt other rules.
#
# We use 'sinclude' (silent-include) rather than plain 'include' to suppress
# the warning if this file doesn't exist yet.
.previous_make_flags: FORCE
	@ \
	path_to() { which "$$1" || echo "$$1 (not found in path)"; }; \
	NEW=$$($$(which echo) -ne \
	       "#TOPDIR='$(TOPDIR)'\n" \
	       "#CPP='$$(path_to $(CPP))'\n" \
	       "#CC='$$(path_to $(CC))'\n" \
	       "#CXX='$$(path_to $(CXX))'\n" \
	       "#CROSS_COMPILE='$(CROSS_COMPILE)'\n" \
	       "#CFLAGS='$(CFLAGS)'\n" \
	       "#CXXFLAGS='$(CXXFLAGS)'\n" \
	       "#CPPFLAGS='$(CPPFLAGS)'\n" \
	       "#LDFLAGS='$(LDFLAGS)'\n" \
	       "#LDLIBS='$(LDLIBS)'\n"); \
	OLD=$$(cat $@ 2>/dev/null); \
	if [ "$$NEW" != "$$OLD" ]; then \
		echo "$$NEW" > $@; \
		if [ -n "$$OLD" ]; then \
			$(MAKE) clean; \
		fi; \
	fi
sinclude .previous_make_flags

# This rule provides a place to do make-related setup operations before any
# real rules are evaluated.  How?  By including the 'file' that is the
# result of this rule (even though it does not exist), we force make to
# evaluate this before the $(MAKECMDGOALS).  Note: in the case of make -j,
# any of these early rules of this rule can run in parallel, including
# .previous_make_flags.
#
# The top-level Makefile can add new deps to this rule, if needed.
.PHONY: build_setup
build_setup: _build_vinclude
sinclude build_setup

# This rule builds a virtual include dir, which allows us to use include
# paths that work in the source tree or in the installed include tree.
.PHONY: _build_vinclude
_build_vinclude:
	@ \
	if [ "$$(readlink .vinclude/pp)" != $(TOPDIR) ]; then \
		mkdir -p .vinclude; \
		ln -sf $(TOPDIR) .vinclude/pp; \
	fi

# The top-level Makefile can define more deps for this rule or actions, if
# it has something it needs done.
.PHONY: clean
clean: _clean_depends _clean_previous_make_flags _clean_vinclude

.PHONY: _clean_depends
_clean_depends:
	@find . -type f -name .\*.d -print0 | xargs -0 $(RM)

.PHONY: _clean_previous_make_flags
_clean_previous_make_flags:
	@$(RM) .previous_make_flags

.PHONY: _clean_vinclude
_clean_vinclude:
	@$(RM) -rf .vinclude

# The top-level Makefile can define more deps for this rule or actions, if
# it has something it needs done.
.PHONY: distclean
distclean: clean

# A generic empty target to force some rules to run.
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

# Include any dependency files we can find.  If none exist, they will get
# created as object files are built.  We're assuming here that nobody has
# done anything stupid like delete all the dependency files from underneath
# us.
include $(shell find . -name .\*.d)
