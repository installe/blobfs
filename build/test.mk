# The following variables should be defined before including this file:
#
#     CXXFLAGS	additional g++ options
#     INCLUDES  source for additional include files
#     LDFLAGS	additional linker flags

INCLUDES := -I$(ROOT)/include $(INCLUDES)
CXXFLAGS := -g3 -Wall -Werror -DPOSIX $(CXXFLAGS) $(INCLUDES) -fPIC
LDFLAGS := -g3 -rdynamic $(LDFLAGS) -fPIC

TST = $(wildcard *.tt)
PKG = $(shell basename `pwd`)
LIBDIR = $(ROOT)/src/$(PKG)
LIB = $(LIBDIR)/lib$(PKG).a

.PHONY: all clean dep build test x

all: test

clean:
	rm -f runtests runtests.cc $~

test: runtests
	./runtests

runtests: runtests.cc $(LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

runtests.cc: $(TST) $(LIB)
	cxxtestgen.pl --error-printer --abort-on-fail --have-eh -o $@ $(TST)

$(LIB): x
	make -C $(LIBDIR) build
