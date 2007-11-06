TOPDIR = ..
include $(TOPDIR)/rules.mk

LIBS += -L$(TOPDIR) -lpp

BINS = pp_discover \
	pp_pci
BINS_SRCS = $(BINS:=.cpp)


all: $(BINS)

$(BINS): %: %.o

.PHONY: test
test: all
	@true

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend $(BINS)

.depend: $(BINS_SRCS)
