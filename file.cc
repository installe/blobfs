#include <iostream>
#include <typeinfo>

#include <boost/shared_ptr.hpp>
#include <log4cpp/Priority.hh>

#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#include "file.hh"
#include "filedata.hh"
#include "filehandle.hh"
#include "fsmarshaller.hh"
#include "NodeAttrMarshaller.hh"
#include "NodeHandleIterator.hh"
#include "NodeMarshaller.hh"
#include "ParentAttrPersistStrategy.hh"
#include "RootDirMarshaller.hh"
#include "Uid.hh"

using namespace std;

class VirtualFileState : public FileState {

private:
    static log4cpp::Category& cat;

    File &file;
    FileHandle fHandle;

    VirtualFileState(VirtualFileState& state) throw();

    VirtualFileState& operator=(const VirtualFileState& state) throw();

public:
    VirtualFileState(File& file, FileHandle fHandle) throw(IoError);

    ~VirtualFileState() throw();

    ssize_t getSize() const throw();

    size_t read(void *buf, ssize_t offset, size_t size) const throw(IoError);

    size_t write(const void *buf, ssize_t offset, size_t size) throw(IoError);

    FileHandle addToDirHandle(DirHandle dirHandle) throw(IoError);

    void release() throw(IoError);

};

class NewFileState : public FileState {

private:
    static log4cpp::Category& cat;

    File &file;
    FileWriter& data;

    NewFileState(NewFileState& state) throw();

    NewFileState& operator=(const NewFileState& state) throw();

public:
    NewFileState(File& file) throw();

    ~NewFileState() throw();

    ssize_t getSize() const throw();

    size_t read(void *buf, ssize_t offset, size_t size) const throw();

    size_t write(const void *buf, ssize_t offset, size_t size) throw();

    FileHandle addToDirHandle(DirHandle dirHandle) throw(IoError);

    void release() throw(IoError);

};

class CachedFileState : public FileState {

private:
    static log4cpp::Category& cat;

    File &file;
    FileHandle fHandle;
    FileWriter& data;

    CachedFileState(CachedFileState& state) throw();

    CachedFileState& operator=(const CachedFileState& state) throw();

public:
    CachedFileState(File &file, FileHandle fHandle, FileWriter& data)
	throw();

    ~CachedFileState() throw();

    ssize_t getSize() const throw();

    size_t read(void *buf, ssize_t offset, size_t size) const throw();

    size_t write(const void *buf, ssize_t offset, size_t size) throw();

    FileHandle addToDirHandle(DirHandle dirHandle) throw(IoError);

    void release() throw();

};

class DirtyFileState : public FileState {

private:
    static log4cpp::Category& cat;

    File &file;
    FileHandle fHandle;
    FileWriter& data;

    DirtyFileState(DirtyFileState& state) throw();

    DirtyFileState& operator=(const DirtyFileState& state) throw();

public:
    DirtyFileState(File &file, FileHandle fHandle, FileWriter& data)
	throw();

    ~DirtyFileState() throw();

    ssize_t getSize() const throw();

    size_t read(void *buf, ssize_t offset, size_t size) const throw();

    size_t write(const void *buf, ssize_t offset, size_t size) throw();

    FileHandle addToDirHandle(DirHandle dirHandle) throw(IoError);

    void release() throw(IoError);

};

// TODO Remove
//VirtualFileState::VirtualFileState(VirtualFileState& state) { }

log4cpp::Category& VirtualFileState::cat =
    log4cpp::Category::getInstance("VirtualFileState");

VirtualFileState::VirtualFileState(File &file, FileHandle fHandle)
    throw(IoError)
    : file(file), fHandle(fHandle)
{
    // TODO Remove
//     FsMarshaller& fsMarshaller = file.getMarshaller();
//     NodeAttrMarshaller attrMarshaller(fsMarshaller);
//     NodeAttr& attr = file.getAttr();

//     attrMarshaller.unmarshallFile(attr, fHandle);
}

VirtualFileState::~VirtualFileState()
    throw() { }

ssize_t VirtualFileState::getSize() const
    throw()
{
    return file.getAttr().getSize();
}

size_t VirtualFileState::read(void *buf, ssize_t offset, size_t size) const
    throw(IoError)
{
    FileWriter& data = file.getMarshaller().loadFile(fHandle);

    file.setState(*new CachedFileState(file, fHandle, data));
    return file.read(buf, offset, size);
}

size_t VirtualFileState::write(const void *buf, ssize_t offset, size_t size)
    throw(IoError)
{
    FileWriter& data = file.getMarshaller().loadFile(fHandle);

    file.setState(*new DirtyFileState(file, fHandle, data));
    file.getParent().setDirty();
    file.write(buf, offset, size);

    return size;
}

FileHandle VirtualFileState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    FsMarshaller& marshaller = file.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    FileHandle newHandle = marshaller.copyFileHandle(dirHandle, fHandle);

    attrMarshaller.marshallFile(file.getAttr(), newHandle);
    return newHandle;
}

void VirtualFileState::release()
    throw(IoError) { }

// TODO Remove
//NewFileState::NewFileState(NewFileState& state) { }

log4cpp::Category& NewFileState::cat =
    log4cpp::Category::getInstance("NewFileState");

/**
 * NOTE: Creating a file should occur using the following sequence:
 *
 *	File& file = new File(parent, name);
 *	parent.addChild(file);
 *
 * As a side effect of the latter statement, the parent parent will go to dirty
 * state. This is necessary to make NewFileState::release() work correctly.
 * _However, this ctor doesn't explicitly set the parent parent to dirty!_
 */
NewFileState::NewFileState(File& file)
    throw()
    : file(file), data(*new FileWriter()) { }

NewFileState::~NewFileState()
    throw() { }

ssize_t NewFileState::getSize() const
    throw()
{
    return data.getSize();
}

size_t NewFileState::read(void *buf, ssize_t offset, size_t size) const
    throw()
{
    data.seek(offset);
    return data.read(buf, size);
}

size_t NewFileState::write(const void *buf, ssize_t offset, size_t size)
    throw()
{
    data.seek(offset);

    size_t numWritten = data.write(buf, size);

    file.getAttr().setSize(data.getSize());
    return numWritten;
}

FileHandle NewFileState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    FsMarshaller& marshaller = file.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    FileHandle newHandle = marshaller.storeFile(dirHandle, data);

    // TODO Include setting name in marshallChild()?
    marshaller.setStringAttr(newHandle, "name", file.getName());
    attrMarshaller.marshallFile(file.getAttr(), newHandle);
    file.setState(*new CachedFileState(file, newHandle, data));
    return newHandle;
}

void NewFileState::release()
    throw(IoError)
{
    NodeAttr& attr = file.getAttr();
    time_t now = time(NULL);

    attr.setCTime(now);
    attr.setMTime(now);
    attr.setATime(now);

    // TODO: Refactor
    // The Node interface should get the following method:
    // _ (public)	commit(): commits a node if dirty
    // The Parent interface should get the following method:
    // _ (internal)	forceCommit(): does the actual work
    // The Child interface should get the following method:
    // _ (internal)	addToDirHandle(): used by the Parent to commit()
    file.getParent().setDirty();
    file.getParent().release();
}

// TODO Can I remove this?
//CachedFileState::CachedFileState(File &file) : file(file) { }

log4cpp::Category& CachedFileState::cat =
    log4cpp::Category::getInstance("CachedFileState");

CachedFileState::CachedFileState(File& file, FileHandle fHandle,
				 FileWriter& data)
    throw()
    : file(file), fHandle(fHandle), data(data) { }

CachedFileState::~CachedFileState()
    throw() { } // TOOD Should release FileHandle

ssize_t CachedFileState::getSize() const throw()
{
    return data.getSize();
}

size_t CachedFileState::read(void *buf, ssize_t offset, size_t size) const
    throw()
{
    data.seek(offset);
    return data.read(buf, size);
}

size_t CachedFileState::write(const void *buf, ssize_t offset, size_t size)
    throw()
{
    file.setState(*new DirtyFileState(file, fHandle, data));
    return file.write(buf, offset, size);
}

FileHandle CachedFileState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    FsMarshaller& marshaller = file.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    FileHandle newHandle = marshaller.copyFileHandle(dirHandle, fHandle);

    attrMarshaller.marshallFile(file.getAttr(), newHandle);
    return newHandle;
}

void CachedFileState::release()
    throw() { }

// TODO Can it be removed?
//DirtyFileState::DirtyFileState(DirtyFileState& state) : file(state.file) { }

log4cpp::Category& DirtyFileState::cat =
    log4cpp::Category::getInstance("DirtyFileState");

DirtyFileState::DirtyFileState(File &file, FileHandle fHandle, FileWriter& data)
    throw()
    : file(file), fHandle(fHandle), data(data) { }

DirtyFileState::~DirtyFileState()
    throw() { }

ssize_t DirtyFileState::getSize() const
    throw()
{
    return data.getSize();
}

size_t DirtyFileState::read(void *buf, ssize_t offset, size_t size) const
    throw()
{
    data.seek(offset);
    return data.read(buf, size);
}

size_t DirtyFileState::write(const void *buf, ssize_t offset, size_t size)
    throw()
{
    data.seek(offset);
    data.write(buf, size);
    file.getAttr().setSize(data.getSize());
    return size;
}

FileHandle DirtyFileState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    FsMarshaller& marshaller = file.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    FileHandle newHandle = marshaller.storeFile(dirHandle, data);

    marshaller.closeFileHandle(fHandle);
    marshaller.setStringAttr(newHandle, "name", file.getName());
    attrMarshaller.marshallFile(file.getAttr(), newHandle);
    file.setState(*new CachedFileState(file, newHandle, data));
    return newHandle;
}

void DirtyFileState::release()
    throw(IoError)
{
    time_t now = time(NULL);
    NodeAttr& attr = file.getAttr();

    attr.setCTime(now);
    attr.setMTime(now);
    attr.setATime(now);

    file.getParent().setDirty();
    file.getParent().release();
}

log4cpp::Category& File::cat = log4cpp::Category::getInstance("File");

File::File(FsMarshaller& fsMarshaller, Parent& parent, const string name)
    throw()
    : Child(parent, name, S_IFREG), marshaller(fsMarshaller),
      state(new NewFileState(*this))
{
    cat.debug("File::File(%s)", getName().c_str());
}

File::File(FsMarshaller& fsMarshaller, Parent& parent, const string name,
	   const FileHandle fHandle)
    throw(IoError)
    : Child(parent, name, S_IFREG), marshaller(fsMarshaller),
      state(new VirtualFileState(*this, fHandle))
{
    cat.debug("File::File(%s, fHandle)", getName().c_str());
}

FsMarshaller& File::getMarshaller() const
    throw()
{
    return marshaller;
}

// TODO Use attr.getSize() for this.
ssize_t File::getSize() const
    throw()
{
    return state->getSize();
}

const FileState& File::getState() const
    throw()
{
    return *state;
}

void File::setState(FileState& _state)
    throw()
{
    delete state;
    state = &_state;
}

size_t File::read(void *buf, ssize_t offset, size_t size)
    throw(IoError)
{
    return state->read(buf, offset, size);
}

size_t File::write(const void *buf, ssize_t offset, size_t size)
    throw(IoError)
{
    return state->write(buf, offset, size);
}

FileHandle File::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    return state->addToDirHandle(dirHandle);
}

void File::release()
    throw(IoError)
{
    state->release();
}

// File::File(File& file)
//     : parent(file.parent), name(file.name), fHandle(file.fHandle), state(file.state.clone()), size(file.size), writer(file.writer)
// {
//      cat.debug("File::File(File&)");
// }

File::~File()
    throw()
{
    delete state;

    cat.debug("File::~File(%s)", getName().c_str());
}

// TODO Exists in ChildIterator as well; remove code duplication
typedef boost::shared_ptr<vector <Child *> > ChildVector;

class VirtualRootDirState : public RootDirState {

private:
    static log4cpp::Category& cat;

    RootDir& rootDir;
    Uid uid;

    ChildVector listChildren(DirHandle dirHandle) const
	throw(IoError);

public:
    VirtualRootDirState(RootDir& rootDir, const Uid uid)
	throw();

    ChildIterator childIterator()
	throw(IoError);

    void addChild(Child& child)
	throw(IoError);

    void removeChild(Child& child)
	throw(IoError);

    void setDirty()
	throw(IoError);

    void release()
	throw();

    ~VirtualRootDirState() throw();

};

class NewRootDirState : public RootDirState {

private:
    static log4cpp::Category& cat;

    RootDir& rootDir;
    ChildVector children;

public:
    NewRootDirState(RootDir& rootDir)
	throw();

    ChildIterator childIterator()
	throw();

    void addChild(Child& child)
	throw();

    void removeChild(Child& child)
	throw();

    void setDirty()
	throw();

    void release()
	throw(IoError);

    ~NewRootDirState() throw();

};

class CachedRootDirState : public RootDirState {

private:
    static log4cpp::Category& cat;

    RootDir& rootDir;
    Uid uid;
    DirHandle dirHandle;
    ChildVector children;

public:
    CachedRootDirState(RootDir& rootDir, const Uid uid, DirHandle dirHandle,
		       ChildVector children)
	throw();

    ChildIterator childIterator()
	throw();

    void addChild(Child& child)
	throw();

    void removeChild(Child& child)
	throw();

    void setDirty()
	throw();

    void release()
	throw();

    ~CachedRootDirState() throw();

};

class DirtyRootDirState : public RootDirState {

private:
    static log4cpp::Category& cat;

    RootDir& rootDir;
    Uid uid;
    DirHandle dirHandle;
    ChildVector children;

public:
    DirtyRootDirState(RootDir& rootDir, const Uid uid, DirHandle dirHandle,
		      ChildVector children)
	throw();

    ChildIterator childIterator()
	throw();

    void addChild(Child& child)
	throw();

    void removeChild(Child& child)
	throw(IoError);

    void setDirty()
	throw();

    void release()
	throw(IoError);

    ~DirtyRootDirState() throw();

};

log4cpp::Category& VirtualRootDirState::cat =
    log4cpp::Category::getInstance("VirtualRootDirState");

VirtualRootDirState::VirtualRootDirState(RootDir& rootDir, const Uid uid)
    throw()
    : rootDir(rootDir), uid(uid)
{
    cat.debug("rootDir:virtual");
}

ChildVector VirtualRootDirState::listChildren(DirHandle dirHandle) const
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    NodeHandleIterator it = marshaller.getHandleIterator(dirHandle);
    ChildVector children(new vector<Child *>);

    while (it.hasNext()) {
	FileHandle handle = it.next();
	NodeMarshaller factory;
	Child& child = factory.newChild(marshaller, rootDir, handle);

	children->push_back(&child);
    }

    return children;
}

ChildIterator VirtualRootDirState::childIterator()
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    rootDir.setState(*new CachedRootDirState(rootDir, uid, dirHandle, children));
    return ChildIterator(children);
}

void VirtualRootDirState::addChild(Child& child)
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    children->push_back(&child);
    rootDir.setState(*new DirtyRootDirState(rootDir, uid, dirHandle, children));
}

void VirtualRootDirState::removeChild(Child& child)
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	Child& c = **it;

	if (c.getName() == child.getName()) {
	    children->erase(it);
	    rootDir.setState(*new DirtyRootDirState(rootDir, uid, dirHandle, children));

	    return;
	}
    }

    rootDir.setState(*new CachedRootDirState(rootDir, uid, dirHandle, children));
    throw IoError(ENOENT, "Path <%s> doesn't exist", child.getName().c_str());
}

void VirtualRootDirState::setDirty()
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    rootDir.setState(*new DirtyRootDirState(rootDir, uid, dirHandle, children));
}

void VirtualRootDirState::release()
    throw() { }

VirtualRootDirState::~VirtualRootDirState()
    throw()
{
    cat.debug("rootDir:exit virtual");
}

log4cpp::Category& NewRootDirState::cat =
    log4cpp::Category::getInstance("NewRootDirState");

NewRootDirState::NewRootDirState(RootDir& rootDir)
    throw()
    : rootDir(rootDir), children(new vector<Child *>)
{
    cat.debug("rootDir:new");
    cat.debug("children initialized with %p", children.get());
}

ChildIterator NewRootDirState::childIterator()
    throw()
{
    cat.debug("returning iterator to %zu children", children->size());

    return ChildIterator(children);
}

void NewRootDirState::addChild(Child& child)
    throw()
{
    children->push_back(&child);
}

void NewRootDirState::removeChild(Child& child)
    throw()
{
    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	Child& c = **it;

	if (c.getName() == child.getName()) {
	    children->erase(it);
	    return;
	}
    }

    throw IoError(ENOENT, "Path <%s> doesn't exist", child.getName().c_str());
}

void NewRootDirState::setDirty()
    throw() { }

void NewRootDirState::release()
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle newDirHandle = marshaller.newDirHandle();

    // TODO: Seems like we only need to iterate over children in RootDir/Dir,
    // so remove childIterator() from the (public) interface.
    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	Child& child = **it;

	child.addToDirHandle(newDirHandle);
    }

    Uid uid = marshaller.storeRootDir(newDirHandle);

    rootDir.getRootDirMarshaller().marshal(rootDir, uid);
    cat.debug("Switching from new to cached; children -> %p", children.get());
    rootDir.setState(*new CachedRootDirState(rootDir, uid, newDirHandle,
					     children));
}

NewRootDirState::~NewRootDirState()
    throw()
{
    cat.debug("rootDir:exit new");
}

log4cpp::Category& CachedRootDirState::cat =
    log4cpp::Category::getInstance("CachedRootDirState");

CachedRootDirState::CachedRootDirState(RootDir& rootDir, const Uid uid, DirHandle dirHandle,
				       ChildVector children)
    throw()
    : rootDir(rootDir), uid(uid), dirHandle(dirHandle), children(children)
{
    cat.debug("rootDir:cached");
    cat.debug("children initialized with %p", children.get());
}

ChildIterator CachedRootDirState::childIterator()
    throw()
{
    cat.debug("children = %p", children.get());
    cat.debug("size = %zu", children.get()->size());

    return ChildIterator(children);
}

void CachedRootDirState::addChild(Child& child)
    throw()
{
    RootDirState& newState =
	*new DirtyRootDirState(rootDir, uid, dirHandle, children);

    rootDir.setState(newState);
    newState.addChild(child);
}

void CachedRootDirState::removeChild(Child& child)
    throw()
{
    RootDirState& newState =
	*new DirtyRootDirState(rootDir, uid, dirHandle, children);

    rootDir.setState(newState);
    newState.removeChild(child);
}

void CachedRootDirState::setDirty()
    throw()
{
    rootDir.setState(*new DirtyRootDirState(rootDir, uid, dirHandle, children));
}

void CachedRootDirState::release()
    throw() { }

CachedRootDirState::~CachedRootDirState()
    throw()
{
    cat.debug("rootDir:cached");
}

log4cpp::Category& DirtyRootDirState::cat =
    log4cpp::Category::getInstance("DirtyRootDirState");

DirtyRootDirState::DirtyRootDirState(RootDir& rootDir, const Uid uid,
				     DirHandle dirHandle, ChildVector children)
    throw()
    : rootDir(rootDir), uid(uid), dirHandle(dirHandle), children(children)
{
    cat.debug("rootDir:dirty");
}

ChildIterator DirtyRootDirState::childIterator()
    throw()
{
    return ChildIterator(children);
}

void DirtyRootDirState::addChild(Child& child)
    throw()
{
    children->push_back(&child);
}

void DirtyRootDirState::removeChild(Child& child)
    throw(IoError)
{
    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	Child& c = **it;

	if (c.getName() == child.getName()) {
	    children->erase(it);
	    delete &c;
	    return;
	}
    }

    throw IoError(ENOENT, "Path <%s> doesn't exist", child.getName().c_str());
}

void DirtyRootDirState::setDirty()
    throw() { }

void DirtyRootDirState::release()
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle newDirHandle = marshaller.newDirHandle();

    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	Child& child = **it;

	cout << "Adding to dirHandle: <" << child.getName() << ">" << endl;
	child.addToDirHandle(newDirHandle);
    }

    Uid uid = marshaller.storeRootDir(newDirHandle);

    rootDir.getRootDirMarshaller().marshal(rootDir, uid);
    // TODO: RAII
    marshaller.closeDirHandle(dirHandle);
    rootDir.setState(*new CachedRootDirState(rootDir, uid, newDirHandle,
					     children));
}

DirtyRootDirState::~DirtyRootDirState()
    throw()
{
    cat.debug("rootdir:exit dirty");
}

log4cpp::Category& RootDir::cat = log4cpp::Category::getInstance("RootDir");

RootDir::RootDir(FsMarshaller& marshaller, const RootDirMarshaller& rootDirMarshaller)
    throw()
    : marshaller(marshaller), rootDirMarshaller(rootDirMarshaller),
      state(new NewRootDirState(*this)),
      attr(NodeAttrImpl(*this, *new ParentAttrPersistStrategy(), S_IFDIR)) { }

RootDir::RootDir(FsMarshaller& marshaller, const RootDirMarshaller& rootDirMarshaller,
		 const Uid uid)
    throw(IoError)
    : marshaller(marshaller), rootDirMarshaller(rootDirMarshaller),
      state(new VirtualRootDirState(*this, uid)),
      attr(NodeAttrImpl(*this, *new ParentAttrPersistStrategy(), S_IFDIR)) { }

RootDir::~RootDir()
    throw()
{
    delete state;
}

RootDir& RootDir::operator=(const RootDir& rootDir)
    throw()
{
    RootDir *p = NULL;

    return *p;
}

NodeAttr& RootDir::getAttr()
    throw()
{
    return attr;
}

const NodeAttr& RootDir::getAttr() const
    throw()
{
    return attr;
}

const Node& RootDir::getOwner() const
    throw()
{
    return *this;
}

FsMarshaller& RootDir::getMarshaller() const
    throw()
{
    return marshaller;
}

const RootDirMarshaller& RootDir::getRootDirMarshaller() const
    throw()
{
    return rootDirMarshaller;
}

const RootDirState& RootDir::getState() const
    throw()
{
    return *state;
}

void RootDir::setState(RootDirState& state)
    throw()
{
    cat.debug("RootDir = %p; Deleting old state = %p", this, this->state);

    delete this->state;
    this->state = &state;

    cat.debug("setting &state to %p", &state);
}

ChildIterator RootDir::childIterator()
    throw(IoError)
{
    cat.debug(__PRETTY_FUNCTION__);
    cat.debug("&state = %p", &state);

    return state->childIterator();
}

void RootDir::addChild(Child& child)
    throw(IoError)
{
    state->addChild(child);
}

void RootDir::removeChild(Child& child)
    throw(IoError)
{
    state->removeChild(child);
}

void RootDir::setDirty()
    throw(IoError)
{
    state->setDirty();
}

void RootDir::release()
    throw(IoError)
{
    state->release();
}

log4cpp::Category& DirMarshallerException::cat = log4cpp::Category::getInstance("DirMarshallerException");

DirMarshallerException::DirMarshallerException(string msg, ...) : msg(msg)
{
    va_list ap;

    va_start(ap, msg);
    cat.logva(log4cpp::Priority::ERROR, msg.c_str(), ap);
    va_end(ap);
}

string DirMarshallerException::getMsg() const { return msg; }
