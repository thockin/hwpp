TOPDIR = ../..
include $(TOPDIR)/rules.mk

SRCS = mem_driver.cpp mem_binding.cpp
OBJS = $(SRCS:.cpp=.o)


all: mem_driver_module.o

mem_driver_module.o: $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: test
test: all
	@$(MAKE) -C tests test

.PHONY: clean
clean:
	@$(RM) *.o *.a .depend
	@$(MAKE) -C tests clean

.depend: $(SRCS)
