#ifndef _FILESYSTEM_HH_
#define _FILESYSTEM_HH_

#include "AccessManager.hh"
#include "Clock.hh"
#include "file.hh"
#include "fsmarshaller.hh"
#include "IoError.hh"
#include "log4cpp/Category.hh"
#include "NodeAttrHelper.hh"
#include "Node.hh"
#include "PathUtils.hh"

#include <fuse.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

class FileSystem {

private:
    static log4cpp::Category& cat;

    FsMarshaller& marshaller;
    AccessManager& accessManager;
    RootDir& rootDir;
    Clock& clock;

    PathUtils pathUtils;
    NodeAttrHelper nodeAttrHelper;

    string fullPath(const Node& node) const
	throw();

    Child *findChild(Parent& parent, const string name) const
	throw();

    Node *findNode(const string path) const
	throw(IoError);

    Node& getNode(const string path) const
	throw(IoError);

    Parent& getParent(const string path) const
	throw(IoError);

    File& getFile(const string path) const
	throw(IoError);

public:
    FileSystem(FsMarshaller& marshaller, AccessManager& accessManager,
	       RootDir& rootDir, Clock& clock)
	throw();

    ~FileSystem()
	throw();

    int readDir(const string path, void *buf, fuse_fill_dir_t filler,
		off_t offset, fuse_file_info *fi) const
	throw(IoError);

    int access(const string path, int mode) const
	throw(IoError);

    int getAttr(const string path, struct stat *stbuf) const
	throw(IoError);

    // TODO Remove?
//     int setAttr(const string path, const struct stat *stbuf) const
// 	throw(IoError);

    int create(const string path, mode_t mode, fuse_file_info *fi) const
	throw(IoError);

    int open(const string path, fuse_file_info *fi) const
	throw(IoError);

    int unlink(const string path) const
	throw(IoError);

    int read(const string path, void *buf, size_t size, off_t offset,
	     fuse_file_info *fi) const
	throw(IoError);

    int write(const string path, const void *buf, size_t size, off_t offset,
	      fuse_file_info *fi) const
	throw(IoError);

    int release(const string path, fuse_file_info *fi) const
	throw(IoError);

    int chmod(const string path, mode_t mode) const
	throw(IoError);

    int chown(const string path, uid_t uid, gid_t gid) const
	throw(IoError);

    int utimens(const string path, const struct timespec tv[2]) const
	throw(IoError);

    int mkdir(const string path, mode_t mode) const
	throw(IoError);

    friend class FileSystemTest;

};

#endif /* _FILESYSTEM_HH_ */
