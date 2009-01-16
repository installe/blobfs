INCLUDES = -I$(HOME)/src/filepoollib/Linux-GCC4/include
OPTIONS = -Wall -Werror -DPOSIX -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 -g3

CXXFLAGS = $(INCLUDES) $(OPTIONS)
#LDFLAGS = -L$(HOME)/src/filepoollib/Linux-GCC4/lib -lFPLibrary32
#LDFLAGS = -L$(HOME)/src/slave/svn/cas/router/trunk/build/stage/cubetools/linux -lFPCore32 -lFPLibrary -lFPParser32 -lFPStreams32 -lFPUtils32 -lFPXML32 -lPAI_module32 -L/usr/lib/libstdc++.so.5 -g
LDFLAGS = -L$(HOME)/src/slave/svn/cas/router/trunk/build/stage/cubetools/linux -Wl,-rpath,$(HOME)/src/slave/svn/cas/router/trunk/build/stage/cubetools/linux -lFPCore32 -lFPLibrary -lFPParser32 -lFPStreams32 -lFPUtils32 -lFPXML32 -lPAI_module32 -llog4cpp -lfuse -lboost_filesystem-mt -lboost_thread-mt -g3

SRC = $(filter-out runtests.cc, $(wildcard *.cc))
### TEMP!
#SRC = filedata.cc filehandle.cc file.cc Uid.cc ChildIterator.cc BlobFsMarshaller.cc BlobFsHandleIterator.cc NodeHandleIterator.cc FileSystem.cc PathUtils.cc IoError.cc operations.cc RootUidMarshaller.cc test_fuse.cc NodeAttrImpl.cc NodeAttrMarshaller.cc Child.cc ChildAttrPersistStrategy.cc ParentAttrPersistStrategy.cc
HDR = $(wildcard *.hh)
TST = $(wildcard *.tt)
OBJ = $(patsubst %.cc, %.o, $(SRC))
LIB = libblobfs.a
#LIBOBJ = $(filter-out test_%.o, $(OBJ))
#APPOBJ = $(filter     test_%.o, $(OBJ))
#TGT = $(patsubst %.o, %, $(APPOBJ))
TGT = test_fuse
### TEMP!
#TGT = $(OBJ)

.PHONY: all clean dep build test

all: build test tags

clean:
	rm -f runtests runtests.cc *.o *~ $(LIB)

dep: .depend

build: $(TGT)

test: runtests
	./runtests

tags: TAGS

$(LIB): $(OBJ)
	ar -ru $@ $^

runtests: runtests.cc $(LIB)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

# TODO: runtests.cc should really depend on $(TST) + the headers that are
#	loaded by the *.tt files iso $(LIB).
runtests.cc: $(TST) $(LIB)
	cxxtestgen.pl --error-printer --abort-on-fail --have-eh -o $@ $(TST)

%: %.o $(LIB)
	$(CXX) $(LDFLAGS) -o $@ $^
	sudo chown root.root $@
	sudo chmod u+s $@

.depend: $(SRC)
	touch .depend
	makedepend -f .depend -- $(CXXFLAGS) $(INCLUDES) -- $(SRC) $(TST) 2> /dev/null

TAGS:
	etags `find -name '*.[ch]'`

include .depend

