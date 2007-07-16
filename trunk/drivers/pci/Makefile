#This is just a test for now

INCLUDES = -I../.. -I../../test

all:  pci_device

pci_device: pci_device.cpp
	$(CXX) -g -Wall -Werror $(INCLUDES) -o $@ $^

test:
	$(MAKE) -C tests clean

clean:
	$(RM) *.o
	$(MAKE) -C tests clean
