TOPDIR = ../..
include $(TOPDIR)/rules.mk

SRCS = io_driver.cpp io_binding.cpp
OBJS = $(SRCS:.cpp=.o)


all: io_driver_module.o

io_driver_module.o: $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: test
test: all
	@$(MAKE) -C tests test

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend
	@$(MAKE) -C tests clean

.depend: $(SRCS)
