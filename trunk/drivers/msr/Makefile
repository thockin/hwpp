TOPDIR = ../..
include $(TOPDIR)/rules.mk

SRCS = msr_driver.cpp msr_binding.cpp
OBJS = $(SRCS:.cpp=.o)


all: msr_driver_module.o

msr_driver_module.o: $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: test
test: all
	@$(MAKE) -C tests test

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend
	@$(MAKE) -C tests clean

.depend: $(SRCS)
