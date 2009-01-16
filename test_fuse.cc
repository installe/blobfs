#include "AccessManagerImpl.hh"
#include "BlobFsMarshaller.hh"
#include "file.hh"
#include "operations.hh"
#include "RootDirMarshallerImpl.hh"
#include "SystemClock.hh"

#include <fstream>
#include <log4cpp/BasicConfigurator.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Priority.hh>
#include <string>

#include <fuse.h>

using namespace std;

log4cpp::Category& cat = log4cpp::Category::getRoot();

int main(int argc, char **argv);

static void setup_logging() {
     log4cpp::FileAppender *fileAppender =
	 new log4cpp::FileAppender("main", "/tmp/test_fuse.log", false);

     log4cpp::BasicConfigurator::configure();
     cat.setPriority(log4cpp::Priority::DEBUG);
     cat.setAdditivity(true);
     cat.addAppender(fileAppender);

     // TODO Enable?
//      std::set_terminate (__gnu_cxx::__verbose_terminate_handler);
}

int main(int argc, char **argv) {
     setup_logging();

     if (argc < 3) {
	  cat.fatal("Usage: %s <address> <mountpoint>", argv[0]);
	  return 1;
     }

     // TODO RAII for new objects? smart_ptr?
     string address = argv[1];
     RootDirMarshallerImpl rootDirMarshaller(".blobfs/rootclip");
     BlobFsMarshaller& marshaller =
	 *new BlobFsMarshaller(address);
     RootDir& rootDir = 
	 rootDirMarshaller.unmarshal(marshaller);
     AccessManager& accessManager = *new AccessManagerImpl();
     Clock& clock = *new SystemClock();

     fs = new FileSystem(marshaller, accessManager, rootDir, clock);

     fuse_main(argc - 1, argv + 1, operations(), NULL);

     return 0;
}
