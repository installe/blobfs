#include "BlobFsMarshaller.hh"
#include "file.hh"
#include "operations.hh"

#include <fstream>
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Priority.hh>
#include <string>

#include <fuse.h>

using namespace std;

log4cpp::Category& cat = log4cpp::Category::getRoot();

Uid loadUid();
void storeUid(Uid uid);
int main(int argc, char **argv);

static void setup_logging() {
     log4cpp::FileAppender *fileAppender =
	 new log4cpp::FileAppender("main", "/tmp/test_fuse.log", false);

     log4cpp::BasicConfigurator::configure();
     cat.setPriority(log4cpp::Priority::DEBUG);
     cat.setAdditivity(true);
     cat.addAppender(fileAppender);
}

RootDir& createRootDir(RootUidMarshaller& rootUidMarshaller,
		       FsMarshaller& fsMarshaller)
    throw(IoError)
{
    Uid uid;

    if (rootUidMarshaller.loadRootUid(uid)) {
	cat.info("Found root clip: <%s>", uid.getClipID());

	return *new RootDir(fsMarshaller, uid);
    }

    cat.warn("Didn't find a rootclip.");

    return *new RootDir(fsMarshaller);
}

int main(int argc, char **argv) {
     setup_logging();

     if (argc < 3) {
	  cat.fatal("Usage: %s <address> <mountpoint>", argv[0]);
	  return 1;
     }

     string address = argv[1];
     RootUidMarshaller rootUidMarshaller("/home/arnaud/.blobfs/rootclip");
     BlobFsMarshaller& marshaller =
	 *new BlobFsMarshaller(rootUidMarshaller, address);
     RootDir& rootDir = createRootDir(rootUidMarshaller, marshaller);

     fs = new FileSystem(marshaller, rootDir);

     fuse_main(argc - 1, argv + 1, operations(), NULL);

     return 0;
}
