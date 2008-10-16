TOPDIR ?= $(shell pwd)/../..
include $(TOPDIR)/pp.mk

DRIVER_TARGET = $(DRIVER_NAME)_driver_module.o

OBJS = $(SRCS:.cpp=.o)


all: $(DRIVER_TARGET)

$(DRIVER_TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -nostdlib -Wl,-r -o $@ $(OBJS)

.PHONY: test
test: all
	@$(MAKE) -C tests test

.PHONY: clean
clean:
	@$(RM) $(OBJS) $(DRIVER_TARGET) *.o
	@$(MAKE) -C tests clean

.PHONY: depclean
depclean:
	@$(MAKE) -C tests depclean

.depend: $(SRCS)
