#include "Child.hh"
#include "ChildIterator.hh"
#include "Dir.hh"
#include "FileSystem.hh"
#include "Parent.hh"
#include "UserContext.hh"

#include <assert.h>
#include <errno.h>

#include <string>

using namespace std;

log4cpp::Category& FileSystem::cat =
    log4cpp::Category::getInstance("FileSystem");

FileSystem::FileSystem(FsMarshaller& marshaller, AccessManager& accessManager,
		       RootDir& rootDir, Clock& clock)
    throw()
    : marshaller(marshaller), accessManager(accessManager), rootDir(rootDir),
      clock(clock), pathUtils()
{
    cat.debug("&rootDir == %p", &rootDir);
}

FileSystem::~FileSystem()
    throw() { }

string FileSystem::fullPath(const Node& node) const
    throw()
{
    string name;

    while (const Child *child = dynamic_cast<const Child *>(&node))
	name = child->getName() + '/' + name;

    return name;
}

Child *FileSystem::findChild(Parent& parent, const string name) const
    throw()
{
    for (ChildIterator it = parent.childIterator(); it.hasNext(); /* */) {
	Child& child = it.next();

	if (child.getName() == name)
	    return &child;
    }

    return NULL;
}

Node *FileSystem::findNode(const string path) const
    throw(IoError)
{
    PathIterator it(path);

    if (!it.hasNext())
	return &rootDir;

    UserContext context;
    Node *node = &rootDir;

    do {
	Parent *parent = dynamic_cast<Parent *>(node);

	if (parent == NULL) {
	    throw IoError(ENOTDIR, "path <%s> is not a directory",
			  fullPath(*node).c_str());
	}

	// TODO Fix
// 	accessManager.checkExecutable(*parent, fullPath(*parent),
// 				      context);
	node = findChild(*parent, it.next());

	if (node == NULL)
	    return NULL;
    } while (it.hasNext());

    return node;
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
    UserContext context;
    Parent& parent = getParent(path);

    accessManager.checkReadable(parent, path, context);
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
	Child& child = it.next();

	cat.debug("found <%s/%s>", path.c_str(), child.getName().c_str());
	if (filler(buf, child.getName().c_str(), NULL, 0) != 0)
	    throw IoError(ENOMEM, "insufficient memory");
    }

    parent.getAttr().setATime(clock.time());
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

    cat.debug("mode for <%s>: %i (address = %p)", path.c_str(), attr.getMode(),
	      &attr);
    attr.getAttr(*stbuf);
    return 0;
}

// TODO Remove?
// int FileSystem::setAttr(const string path, const struct stat *stbuf) const
//     throw(IoError)
// {
//     cat.debug("setting attributes for <%s>", path.c_str());

//     Node& node = getNode(path);
//     NodeAttr& attr = node.getAttr();

//     attr.setAttr(*stbuf);
//     attr.persist();
//     return 0;
// }

int FileSystem::create(const string path, mode_t mode, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("creating file <%s>", path.c_str());

    UserContext context;
    Node *node = findNode(path);

    if (node != NULL)
	throw IoError(EEXIST, "path <%s> already exists", path.c_str());

    string parentPath = pathUtils.dirName(path);
    Parent& parent = getParent(parentPath);

    accessManager.checkWritable(parent, "/", context);

    File& file = *new File(marshaller, parent, pathUtils.baseName(path));
    NodeAttr& attr = file.getAttr();
    time_t now = clock.time();

    parent.addChild(file);
    attr.setUid(context.getUid());
    attr.setGid(context.getGid());
    nodeAttrHelper.setPerms(attr, mode);
    attr.setCTime(now);
    attr.setMTime(now);
    attr.setATime(now);
    parent.getAttr().setMTime(now);

    fi->fh = (uint64_t) &file;

    cat.debug("created file <%s> successfully", path.c_str());
    return 0;
}

int FileSystem::open(const string path, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("opening file <%s>", path.c_str());

    UserContext context;
    File& file = getFile(path);
    int accessMode = fi->flags & O_ACCMODE;

    if (accessMode == O_RDONLY) {
	accessManager.checkReadable(file, path, context);
    } else if (accessMode == O_WRONLY) {
	accessManager.checkWritable(file, path, context);
    } else if (accessMode == O_RDWR) {
	accessManager.checkReadable(file, path, context);
	accessManager.checkWritable(file, path, context);
    } else {
	throw IoError(EINVAL, "Invalid access mode: %i", accessMode);
    }

    // TODO Change solution to avoid casting - How?
    // TODO We should fill this in with a class that contains a reference to
    // a file + the access mode that was used for opening, so we can check
    // access permissions later on!
    fi->fh = (uint64_t) &file;

    cat.debug("opened file <%s> successfully", path.c_str());
    return 0;
}

int FileSystem::unlink(const string path) const
    throw(IoError)
{
    cat.debug("unlinking file <%s>", path.c_str());

    UserContext context;
    File& file = getFile(path);
    Parent& parent = file.getParent();

    accessManager.checkModifiable(file, path, context);
    parent.removeChild(file);
    parent.getAttr().setMTime(clock.time());
    parent.release();

    return 0;
}

int FileSystem::read(const string path, void *buf, size_t size, off_t offset,
		     fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("reading file <%s> (offset = %i, size = %zu)",
	      path.c_str(), offset, size);

    File& file = *((File *) fi->fh);

    // TODO Verify that the file was opened for reading
    assert(file.getName() == pathUtils.baseName(path));
    accessManager.checkReadable(file, path, UserContext());
    file.getAttr().setATime(clock.time());

    return file.read(buf, offset, size);
}

int FileSystem::write(const string path, const void *buf, size_t size,
		      off_t offset, fuse_file_info *fi) const
    throw(IoError)
{
    cat.debug("writing file <%s> (offset = %i, size = %zu)",
	      path.c_str(), offset, size);

    File& file = *((File *) fi->fh);
    time_t now = clock.time();

    // TODO Verify that the file was opened for writing
    assert(file.getName() == pathUtils.baseName(path));
    accessManager.checkWritable(file, path, UserContext());
    file.getAttr().setATime(now);
    file.getAttr().setMTime(now);
    file.getAttr().setCTime(now);

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

    UserContext context;
    Node& node = getNode(path);
    NodeAttr& attr = node.getAttr();

    accessManager.checkModifiable(node, path, context);
    nodeAttrHelper.setPerms(attr, mode);
    attr.setCTime(clock.time());
    attr.persist();

    return 0;
}

int FileSystem::chown(const string path, uid_t uid, gid_t gid) const
    throw(IoError)
{
    cat.debug("changing ownership for file <%s> to %i:%i", path.c_str(), uid, gid);

    UserContext context;
    Node& node = getNode(path);
    NodeAttr& attr = node.getAttr();

    accessManager.checkModifiable(node, path, context);
    accessManager.checkWritable(node.getOwner(), path, context);

    if (uid != static_cast<unsigned>(-1)) attr.setUid(uid);
    if (gid != static_cast<unsigned>(-1)) attr.setGid(gid);
    attr.setCTime(clock.time());
    attr.persist();

    return 0;
}

static time_t determineTime(time_t current, const struct timespec tv)
    throw()
{
    if (tv.tv_nsec == UTIME_NOW)
	return time(NULL);

    if (tv.tv_nsec == UTIME_OMIT)
	return current;

    return tv.tv_sec;
}

int FileSystem::utimens(const string path, const struct timespec tv[2]) const
    throw(IoError)
{
    cat.debug("setting access/modification times for <%s>", path.c_str());

    UserContext context;
    Node& node = getNode(path);
    NodeAttr& attr = node.getAttr();

    accessManager.checkModifiable(node, path, context);

    if (tv == NULL) {
	time_t now = time(NULL);

	attr.setATime(now);
	attr.setMTime(now);
    } else {
	attr.setATime(determineTime(attr.getATime(), tv[0]));
	attr.setMTime(determineTime(attr.getMTime(), tv[1]));
    }

    attr.persist();
    return 0;
}

int FileSystem::mkdir(const string path, mode_t mode) const
    throw(IoError)
{
    cat.debug("creating directory <%s>", path.c_str());

    string parentPath = pathUtils.dirName(path);
    UserContext context;
    Parent& parent = getParent(parentPath);

    accessManager.checkWritable(parent, parentPath, context);

    Dir& dir = *new Dir(marshaller, parent, pathUtils.baseName(path));
    NodeAttr& attr = dir.getAttr();
    time_t now = clock.time();

    parent.addChild(dir);
    parent.getAttr().setMTime(now);
    attr.setUid(context.getUid());
    attr.setGid(context.getGid());
    nodeAttrHelper.setPerms(attr, mode);
    attr.setCTime(now);
    attr.setMTime(now);
    attr.setATime(now);
    dir.release();

    cat.debug("created dir <%s> successfully", path.c_str());
    return 0;
}
