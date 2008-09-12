TOPDIR = ..
include $(TOPDIR)/rules.mk

BINS = pp_discover \
	pp_pci \
	pp_read \
	pp_fuse \
	pp_server \
	pp_client
BINS_SRCS = $(BINS:=.cpp)

pp_fuse_LDLIBS = -lfuse
pp_fuse_LDLIBS_DYN = -lpthread
ifeq ($(strip $(STATIC)),1)
    pp_fuse_LDLIBS += -ldl -lrt
endif

all: $(BINS)

$(BINS): %: %.o $(TOPDIR)/libpp.a cmdline.o

.PHONY: test
test: all
	@true

.PHONY: clean
clean:
	@$(RM) $(OBJS) *.o *.a $(BINS)

.depend: $(BINS_SRCS)
