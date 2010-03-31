TOPDIR ?= $(shell pwd)/..
include $(TOPDIR)/pp.mk


SRCS = language.cpp \
       variable.cpp \
       auto.lex.cpp \
       auto.yacc.cpp
OBJS = $(SRCS:.cpp=.o)


all: language.a

language.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
	$(RANLIB) $@

auto.lex.cpp auto.lex.h: grammar.l
	$(LEX) -o auto.lex.cpp --header-file=auto.lex.h $<
auto_lex_o_CWARNS = -Wno-unused -Wno-sign-compare

auto.yacc.cpp auto.yacc.h: grammar.y
	$(YACC) -o auto.yacc.cpp --defines=auto.yacc.h $<
auto_yacc_o_CXXWARNS = -Wno-write-strings

.PHONY: test
test: all
	$(CXX) -o main_parse main_parse.cpp *.o -g -I.. -lgmpxx -lgmp
	@$(MAKE) -C tests test

.PHONY: clean
clean:
	@$(RM) $(OBJS) auto.*.* *.o *.a
	@$(MAKE) -C tests clean

.PHONY: depclean
depclean: clean
	@$(MAKE) -C tests depclean

.PHONY: distclean
distclean: clean
	@$(MAKE) -C tests distclean

.depend: $(SRCS)
