ROOT = ../..
INCLUDES = -I$(HOME)/src/filepoollib/Linux-GCC4/include -I$(ROOT)/include
OPTIONS = -Wall -Werror -DPOSIX -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 -g3

CXXFLAGS = $(INCLUDES) $(OPTIONS)
#LDFLAGS = -L$(HOME)/src/filepoollib/Linux-GCC4/lib -lFPLibrary32
#LDFLAGS = -L$(HOME)/src/slave/svn/cas/router/trunk/build/stage/cubetools/linux -lFPCore32 -lFPLibrary -lFPParser32 -lFPStreams32 -lFPUtils32 -lFPXML32 -lPAI_module32 -L/usr/lib/libstdc++.so.5 -g
#LDFLAGS = -L$(HOME)/src/slave/svn/cas/router/trunk/build/stage/cubetools/linux -Wl,-rpath,$(HOME)/src/slave/svn/cas/router/trunk/build/stage/cubetools/linux -lFPCore32 -lFPLibrary -lFPParser32 -lFPStreams32 -lFPUtils32 -lFPXML32 -lPAI_module32 -llog4cpp -lfuse -lboost_filesystem-mt -lboost_thread-mt -g3

SRC = $(wildcard *.cc)
OBJ = $(patsubst %.cc, %.o, $(SRC))
#LIB = libblobfs.a
LIB = libbuf.a
#LIBOBJ = $(filter-out test_%.o, $(OBJ))
#APPOBJ = $(filter     test_%.o, $(OBJ))
#TGT = $(patsubst %.o, %, $(APPOBJ))
#TGT = test_fuse
### TEMP!
#TGT = $(OBJ)

.PHONY: all clean dep build test

all: build

clean:
	rm -f *.o *~ $(LIB)

dep: .depend

build: $(LIB)

$(LIB): $(OBJ)
	ar -ru $@ $^

.depend: $(SRC)
	touch .depend
	makedepend -f .depend -- $(CXXFLAGS) $(INCLUDES) -- $(SRC) $(TST) 2> /dev/null

include .depend
