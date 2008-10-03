#include "FileSystem.hh"
#include "Child.hh"
#include "ChildIterator.hh"
#include "Parent.hh"

#include <assert.h>
#include <errno.h>

#include <string>

using namespace std;

log4cpp::Category& FileSystem::cat =
    log4cpp::Category::getInstance("FileSystem");

FileSystem::FileSystem(FsMarshaller& marshaller, RootDir& rootDir)
    throw()
    : marshaller(marshaller), rootDir(rootDir), pathUtils()
{
    cat.debug("&rootDir == %p", &rootDir);
}

FileSystem::~FileSystem()
    throw() { }

Node* FileSystem::findNode(const string path) const
    throw(IoError)
{
    PathIterator it(path);

    if (!it.hasNext())
	return &rootDir;

    string part = it.next();

    for (ChildIterator it = rootDir.childIterator(); it.hasNext(); /* */) {
	Child* child = it.next();

	if (child->getName() == part)
	    return child;
    }

    return NULL;
}

Node& FileSystem::getNode(const string path) const
    throw(IoError)
{
    cat.debug("looking up checked node <%s>", path.c_str());

    Node *node = findNode(path);

    if (node == NULL) {
	throw IoError(ENOENT, "can't find node <%s> on partition",
		      path.c_str());
    }

    cat.debug("found checked node <%s>", path.c_str());

    return *node;
}

Parent& FileSystem::getParent(const string path) const
    throw(IoError)
{
    cat.debug("looking up checked parent <%s>", path.c_str());

    Node *node = findNode(path);

    if (node == NULL) {
	throw IoError(ENOENT, "can't find directory <%s> on partition",
		      path.c_str());
    }

    Parent *parent = dynamic_cast<Parent *>(node);

    if (parent == NULL) {
	throw IoError(ENOTDIR, "<%s> is not a directory on partition",
		      path.c_str());
    }

    cat.debug("found checked parent <%s>", path.c_str());

    return *parent;
}

File& FileSystem::getFile(const string path) const
    throw(IoError)
{
    cat.debug("looking up checked child <%s>", path.c_str());

    Node *node = findNode(path);

    if (node == NULL) {
	throw IoError(ENOENT, "can't find file <%s> on partition",
		      path.c_str());
    }

    File *file = dynamic_cast<File *>(node);

    if (file == NULL) {
	throw IoError(EISDIR, "<%s> is not a regular file on partition",
		      path.c_str());
    }

    cat.debug("found checked file <%s>", path.c_str());

    return *file;
}

int FileSystem::readDir(const string path, void *buf, fuse_fill_dir_t filler,
			off_t offset, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("reading directory <%s>", path.c_str());

    // TODO Keep state in between readDir calls
    Parent& parent = getParent(path);

    cat.debug("Found parent: %p", &parent);

    if (filler(buf, ".", NULL, 0) != 0)
	throw IoError(ENOMEM, "insufficient memory");
    if (filler(buf, "..", NULL, 0) != 0)
	throw IoError(ENOMEM, "insufficient_memory");

    cat.debug("filled in '.' directories");

    unsigned i = 2;

    cat.debug("%p parent", &parent);
    cat.debug("dereferencing parent = %i", &parent);

    ChildIterator it = parent.childIterator();

    cat.debug("got ChildIterator");

    for (ChildIterator it = parent.childIterator(); it.hasNext(); i++) {
	Child *child = it.next();

	cat.debug("found <%s/%s>", path.c_str(), child->getName().c_str());
	if (filler(buf, child->getName().c_str(), NULL, 0) != 0)
	    throw IoError(ENOMEM, "insufficient memory");
    }
    
    cat.debug("read directory <%s> with %i entries", path.c_str(), i);
    return 0;
}

int FileSystem::access(const string path, int mode) const
    throw(IoError)
{
    cat.debug("checking access for %s; mod = %i", path.c_str(), mode);

    // TODO Implement
//     Node& node = getNode(path);
//     NodeAttr& attr = node.getAttr();

//     if (attr.getUid()

    return 0;
}

int FileSystem::getAttr(const string path, struct stat *stbuf) const
    throw(IoError)
{
    cat.debug("getting attributes for <%s>", path.c_str());

    Node &node = getNode(path);
    NodeAttr& attr = node.getAttr();

    attr.getAttr(*stbuf);
    return 0;

    // TODO Refactor: Node should beget a getAttr() method
//     if (File *file = dynamic_cast<File *>(node)) {
// 	cat.debug(" found a file");
// 	stbuf->st_mode = S_IFREG | S_IRWXU | S_IRWXG | S_IRWXO;
// 	stbuf->st_nlink = 1;
// 	stbuf->st_size = file->getSize();
//     } else if (dynamic_cast<RootDir *>(node) != NULL) {
// 	cat.debug(" found a directory");
// 	stbuf->st_mode =
// 	    S_IFDIR | S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH;
// 	stbuf->st_nlink = 2;
// 	stbuf->st_size = 0;
//     } else {
// 	cat.debug(" huh?");
// 	assert(false); // never reached
//     }

//     cat.debug("got attributes for <%s>", path.c_str());
    return 0;
}

int FileSystem::create(const string path, mode_t mode, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("creating file <%s>", path.c_str());

    Node *node = findNode(path);

    if (node != NULL)
	throw IoError(EEXIST, "path <%s> already exists", path.c_str());

    File& file = *new File(marshaller, rootDir, pathUtils.baseName(path));

    rootDir.addFile(file);
    file.getAttr().setMode(mode);

    fi->fh = (uint64_t) &file;

    cat.debug("created file <%s> successfully", path.c_str());
    return 0;
}

int FileSystem::open(const string path, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("opening file <%s>", path.c_str());

    File& file = getFile(path);

    // TODO Change solution to avoid casting
    fi->fh = (uint64_t) &file;

    cat.debug("opened file <%s> successfully", path.c_str());
    return 0;
}

int FileSystem::unlink(const string path) const
    throw(IoError)
{
    cat.debug("unlinking file <%s>", path.c_str());

    File& file = getFile(path);
    Parent& parent = file.getParent();

    parent.removeFile(file);

    return 0;
}

int FileSystem::read(const string path, void *buf, size_t size, off_t offset,
		     fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("reading file <%s> (offset = %i, size = %zu)",
	      path.c_str(), offset, size);

    File& file = *((File *) fi->fh);

    assert(file.getName() == pathUtils.baseName(path));

    return file.read(buf, offset, size);
}

int FileSystem::write(const string path, const void *buf, size_t size,
		      off_t offset, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("writing file <%s> (offset = %i, size = %zu)",
	      path.c_str(), offset, size);

    File& file = *((File *) fi->fh);

    assert(file.getName() == pathUtils.baseName(path));

    return file.write(buf, offset, size);
}

int FileSystem::release(const string path, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("releasing file <%s>", path.c_str());

    File& file = *((File *) fi->fh);

    assert(file.getName() == pathUtils.baseName(path));

    file.release();

    return 0;
}

int FileSystem::chmod(const string path, mode_t mode) const
    throw(IoError)
{
    cat.debug("changing permissions for file <%s>", path.c_str());

    Node& node = getNode(path);
    NodeAttr& attr = node.getAttr();

    attr.setMode(mode);
    attr.persist();

    return 0;
}

int FileSystem::chown(const string path, uid_t uid, gid_t gid) const
    throw(IoError)
{
    cat.debug("changing ownership for file <%s> to %i:%i", path.c_str(), uid, gid);

    Node& node = getNode(path);
    NodeAttr& attr = node.getAttr();

    attr.setUid(uid);
    attr.setGid(gid);
    attr.persist();

    return 0;
}
