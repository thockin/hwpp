TOPDIR = ..
include $(TOPDIR)/rules.mk

BINS = pp_discover \
	pp_pci \
	pp_read \
	pp_fuse
BINS_SRCS = $(BINS:=.cpp)

pp_fuse_LDLIBS = -lfuse -lpthread

all: $(BINS)

$(BINS): %: %.o $(TOPDIR)/libpp.a

.PHONY: test
test: all
	@true

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a .depend $(BINS)

.depend: $(BINS_SRCS)
