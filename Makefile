TOPDIR := $(shell pwd)
include HWPP.mk

# These variables get added to by modules.
SRCS :=
OBJS = $(SRCS:.cc=.o)  # This is intentionally not a := variable.
BINS :=
TESTS :=
CLEANS := libhwpp.a
DISTCLEANS :=

# This is a private helper for IMPORT_MODULE.
define _IMPORT_MODULE
  include $(1)/Make.module
endef
# Import a sub-module's make targets.
IMPORT_MODULE = $(eval $(call _IMPORT_MODULE,$(strip $(1))))

# Import all of the sub-modules.
$(call IMPORT_MODULE, .)
$(call IMPORT_MODULE, devices)
$(call IMPORT_MODULE, drivers)
$(call IMPORT_MODULE, examples)
$(call IMPORT_MODULE, language)
$(call IMPORT_MODULE, util)

all: $(BINS) $(TESTS)

$(BINS): %: %.o libhwpp.a
$(TESTS): %: %.o
CLEANS += $(BINS) $(BINS:=.o) $(TESTS) $(TESTS:=.o)

libhwpp.a: $(OBJS)
	$(RM) $@
	ar rcs $@ $^
	ranlib $@

.PHONY: test
test: $(TESTS)
	@$(MAKE) run_tests RUN_TESTS="$(TESTS)"

.PHONY: clean
clean:
	@$(RM) $(OBJS) $(CLEANS)

.PHONY: distclean
distclean: clean
	@$(RM) $(BUILD_CONFIG) $(DISTCLEANS)
