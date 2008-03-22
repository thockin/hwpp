TOPDIR = ../..
include $(TOPDIR)/rules.mk

DRIVER_TARGET = $(DRIVER_NAME).so

SRCS = $(DRIVER_NAME)_driver.cpp $(DRIVER_NAME)_binding.cpp
OBJS = $(SRCS:.cpp=.do)


all: $(DRIVER_TARGET)

$(DRIVER_TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) -shared -fPIC -o $@ $^

.PHONY: test
test: all
	@$(MAKE) -C tests test

.PHONY: clean
clean:
	@$(RM) $(OBJS) $(DRIVER_TARGET) *.o .depend
	@$(MAKE) -C tests clean

.depend: $(SRCS)
