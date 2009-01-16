#include "file.hh"
#include "FileSystem.hh"
#include "fsmarshaller.hh"
#include "IoError.hh"
#include "operations.hh"

#include <errno.h>
#include <fuse.h>

using namespace std;

FileSystem *fs;

static log4cpp::Category& cat = log4cpp::Category::getInstance("operations");

static int fs_readDir(const char *path, void *buf, fuse_fill_dir_t filler,
		      off_t offset, struct fuse_file_info *fi)
    throw();

static int fs_getAttr(const char *path, struct stat *stbuf)
    throw();

static int fs_readDir(const char *path, void *buf, fuse_fill_dir_t filler,
		      off_t offset, struct fuse_file_info *fi)
    throw()
{
    try {
	return fs->readDir(path, buf, filler, offset, fi);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_getAttr(const char *path, struct stat *stbuf)
    throw()
{
    try {
	return fs->getAttr(path, stbuf);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

// TODO Remove?
// static int fs_setAttr(const char *path, const struct stat *stbuf)
//     throw()
// {
//     try {
// 	return fs->setAttr(path, stbuf);
//     } catch (IoError e) {
// 	cat.error(e.what());
// 	return -e.errcode();
//     } catch (...) {
// 	cat.error("Caught unexpected exception");
// 	return -EFAULT;
//     }
// }

static int fs_open(const char *path, struct fuse_file_info *fi)
    throw()
{
    try {
	return fs->open(path, fi);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_create(const char *path, mode_t mode, struct fuse_file_info *fi)
    throw()
{
    try {
	return fs->create(path, mode, fi);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_unlink(const char *path)
    throw()
{
    try {
	return fs->unlink(path);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_read(const char *path, char *buf, size_t size, off_t offset,
		   struct fuse_file_info *fi)
    throw()
{
    try {
	return fs->read(path, buf, size, offset, fi);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_write(const char *path, const char *buf, size_t size,
		    off_t offset, struct fuse_file_info *fi)
    throw()
{
    try {
	return fs->write(path, buf, size, offset, fi);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_release(const char *path, struct fuse_file_info *fi)
    throw()
{
    try {
	return fs->release(path, fi);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_access(const char *path, int mode)
    throw()
{
    try {
	return fs->access(path, mode);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_chmod(const char *path, mode_t mode)
    throw()
{
    try {
	return fs->chmod(path, mode);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }
}

static int fs_chown(const char *path, uid_t uid, gid_t gid)
    throw()
{
    try {
	fs->chown(path, uid, gid);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }

    return 0;
}

static int fs_utimens(const char *path, const struct timespec tv[2])
    throw()
{
    try {
	fs->utimens(path, tv);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }

    return 0;
}

static int fs_mkdir(const char *path, mode_t mode)
    throw(IoError)
{
    try {
	fs->mkdir(path, mode);
    } catch (IoError e) {
	cat.error(e.what());
	return -e.errcode();
    } catch (...) {
	cat.error("Caught unexpected exception");
	return -EFAULT;
    }

    return 0;
}

fuse_operations *operations()
    throw()
{
     fuse_operations *oper = new fuse_operations();

     oper->readdir = fs_readDir;
     oper->access = fs_access;
     oper->getattr = fs_getAttr;
     // TODO Remove?
//     oper->setattr = fs_setAttr;
     oper->open = fs_open;
     oper->create = fs_create;
     oper->unlink = fs_unlink;
     oper->read = fs_read;
     oper->write = fs_write;
     oper->release = fs_release;
     oper->chmod = fs_chmod;
     oper->chown = fs_chown;
     oper->utimens = fs_utimens;
     oper->mkdir = fs_mkdir;

     return oper;
}
