# The following variables should be defined before including this file:
#
#     CXXFLAGS	additional g++ options
#     INCLUDES  source for additional include files
#     LDFLAGS	additional linker flags

INCLUDES := -I$(ROOT)/include $(INCLUDES)
CXXFLAGS := -g3 -Wall -Werror -DPOSIX $(CXXFLAGS) $(INCLUDES) -fPIC
LDFLAGS := -g3 -rdynamic $(LDFLAGS) -fPIC

SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
PKG = $(shell basename `pwd`)
LIB = lib$(PKG).a
TSTDIR = $(ROOT)/test/$(PKG)

.PHONY: all clean dep build

all: build test

clean:
	rm -f *.o $(LIB)

dep: .depend

build: $(LIB)

test:
	make -C $(TSTDIR)

$(LIB): $(OBJ)
	ar -ru $@ $^

.depend: $(SRC)
	touch .depend
	makedepend -f .depend -- $(CXXFLAGS) $(INCLUDES) -- $(SRC) 2> /dev/null

include .depend
