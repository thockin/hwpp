# our build config file
BUILD_CONFIG = $(TOPDIR)/BUILD_CONFIG


# project version
PRJ_VER_MAJOR = 0
PRJ_VER_MINOR = 2
PRJ_VER_MICRO = 0


# assign build options default values
# Use '?=' variable assignment so ENV variables can be used.

DEBUG ?= 1		# boolean: 0=no, 1=yes
STATIC ?= 0		# option: 0=no, 1=yes, 2=partial
PROFILE ?= 0		# boolean: 0=no, 1=yes


# assign any flags variables
# Use '+=' variable assignment so ENV variables can be used.

DEFS += -D_GNU_SOURCE
LIBS += -lgmpxx -lgmp
LIBS_DYN += -lstdc++
MAKEFLAGS += --no-print-directory


# include the generic rules
include $(TOPDIR)/rules.mk
