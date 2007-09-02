INCLUDES = -I..
CXXFLAGS = -Wall -Werror $(INCLUDES) -g

SRCS = global.cpp \
	pci/pci.cpp

OBJS = $(SRCS:.cpp=.o)


all: all_devices.o

all_devices.o: .depend $(OBJS)
	$(LD) -r -o $@ $(OBJS)

.PHONY: test
test:

.PHONY: clean
clean:
	$(RM) *.o $(OBJS)

.PHONY: dep
dep .depend: $(SRCS)
	@for f in $(SRCS); do \
		$(CPP) $(INCLUDES) -MM $$f; \
	done > .depend

ifeq (.depend,$(wildcard .depend))
include .depend
endif
