CXXFLAGS = -Wall -Werror -DPOSIX
LDFLAGS = -g3 -lboost_filesystem-mt

SRC = $(wildcard *.cc)
HDR = $(wildcard *.hh)
OBJ = $(patsubst %.cc, %.o, $(SRC))
LIB = libblobfs-test.a
TGT = test

.PHONY: all clean dep build test

all: build

clean:
	rm -f *.o *~

dep: .depend

build: $(TGT)

$(TGT): $(LIB)
	$(CXX) $(LDFLAGS) -o $@ $^

$(LIB): $(OBJ)
	ar -ru $@ $^

.depend: $(SRC)
	touch .depend
	makedepend -f .depend -- $(CXXFLAGS) $(INCLUDES) -- $(SRC) 2> /dev/null

include .depend

