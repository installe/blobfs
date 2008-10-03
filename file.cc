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
#include "ParentAttrPersistStrategy.hh"
#include "ChildIterator.hh" // TODO Remove?
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
    FsMarshaller& fsMarshaller = file.getMarshaller();
    NodeAttrMarshaller attrMarshaller(fsMarshaller);
    NodeAttr& attr = file.getAttr();

    attrMarshaller.unmarshallFile(attr, fHandle);
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
 *	parent.addFile(file);
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

    // TODO Include setting name in marshallFile()?
    marshaller.setFileStringAttr(newHandle, "fName", file.getName());
    attrMarshaller.marshallFile(file.getAttr(), newHandle);
    file.setState(*new CachedFileState(file, newHandle, data));
    return newHandle;
}

void NewFileState::release()
    throw(IoError)
{
    file.getParent().release();
//     FsMarshaller marshaller = file.getMarshaller();
//     DirHandle dirHandle = marshaller.newDirHandle();

//     for (ChildIterator it = file.getParent().childIterator(); it.hasNext(); /* */) {
// 	File &file = it.next();

// 	marshaller.addFileHandle(file.addToDirHandle(dirHandle));
//     }

    

//     FsMarshaller marshaller = file.getMarshaller();
//     DirHandle dirHandle = 

//     FileHandle fHandle = file.getMarshaller().storeFile(dirHandle, data);

//     file.setState(*new CachedFileState(file, fHandle, data));
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
    throw() { }

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
    marshaller.setFileStringAttr(newHandle, "fName", file.getName());
    attrMarshaller.marshallFile(file.getAttr(), newHandle);
    file.setState(*new CachedFileState(file, newHandle, data));
    return newHandle;
}

void DirtyFileState::release()
    throw(IoError)
{
    file.getParent().release();

//     FsMarshaller& marshaller = file.getParent().getMarshaller();
//     DirHandle dirHandle = marshaller.newDirHandle();
//     FileHandle fHandle = file.getMarshaller().storeFile(dirHandle, data);

//     file.setState(*new CachedFileState(file, fHandle, data));
//     file.getParent().release();
}

log4cpp::Category& File::cat = log4cpp::Category::getInstance("File");

File::File(FsMarshaller& fsMarshaller, Parent& parent, const string name)
    throw()
    : marshaller(fsMarshaller), parent(parent), name(name),
      state(new NewFileState(*this))
{
    cat.debug("File::File(%s)", name.c_str());
}

File::File(FsMarshaller& fsMarshaller, Parent& parent, const string name,
	   const FileHandle fHandle)
    throw(IoError)
    : marshaller(fsMarshaller), parent(parent), name(name),
      state(new VirtualFileState(*this, fHandle))
{
    cat.debug("File::File(%s, fHandle)", name.c_str());
}

FsMarshaller& File::getMarshaller() const
    throw()
{
    return marshaller;
}

Parent& File::getParent() const
    throw()
{
    return parent;
}

const string File::getName() const
    throw()
{
    return name;
}

// TODO Remove?
// NodeAttr& File::getAttr()
//     throw()
// {
//     Child::getAttr().setMode((Child::getAttr().getMode() | S_IFREG) & ~S_IFDIR);

//     return Child::getAttr();
// }

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
    cat.debug("File::~File(%s)", name.c_str());
}

// void File::setSize(size_t _size) { size = _size; }

// size_t File::getSize() const { return size; }

// void File::setCached(bool _cached) { cached = _cached; }

// bool File::isCached() const { return cached; }

// bool File::isDirty() const { return dirty; }

// FileHandle File::getFileHandle() const { return fHandle; }

// void File::setFileHandle(const FileHandle _fHandle) { fHandle = _fHandle; }

// void File::setDirty(bool _dirty)
// {
//     dirty = _dirty;

//     cat.debug("Set file <%s> to %s", name.c_str(), dirty ? "dirty" : "clean");

//     if (dir != NULL)
// 	dir->setDirty(true);
// }

// FileWriter* File::getWriter()
// {
//     if (writer == NULL)
// 	writer = new FileWriter();

//     return writer;
// }

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

    void addFile(File& file)
	throw(IoError);

    void removeFile(File& file)
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

    void addFile(File& file)
	throw();

    void removeFile(File& file)
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

    void addFile(File& file)
	throw();

    void removeFile(File& file)
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

    void addFile(File& file)
	throw();

    void removeFile(File& file)
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
	FileHandle fHandle = it.next();
	// TODO Call getFileStringAttr() from File ctor ?
	string fName = marshaller.getFileStringAttr(fHandle, "fName");
	File& file = *new File(marshaller, rootDir, fName, fHandle);

	children->push_back(&file);
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

void VirtualRootDirState::addFile(File& file)
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    children->push_back(&file);
    rootDir.setState(*new DirtyRootDirState(rootDir, uid, dirHandle, children));
}

void VirtualRootDirState::removeFile(File& file)
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	File& f = dynamic_cast<File&>(**it);

	if (f.getName() == file.getName()) {
	    children->erase(it);
	    rootDir.setState(*new DirtyRootDirState(rootDir, uid, dirHandle, children));

	    return;
	}
    }

    rootDir.setState(*new CachedRootDirState(rootDir, uid, dirHandle, children));
    throw IoError(ENOENT, "File %s doesn't exist", file.getName().c_str());
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

void NewRootDirState::addFile(File& file)
    throw()
{
    children->push_back(&file);
}

void NewRootDirState::removeFile(File& file)
    throw()
{
    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	File& f = dynamic_cast<File&>(**it);

	if (f.getName() == file.getName()) {
	    children->erase(it);
	    return;
	}
    }

    throw IoError(ENOENT, "File %s doesn't exist", file.getName().c_str());
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
	File& file = dynamic_cast<File&>(**it);

	file.addToDirHandle(newDirHandle);
    }

    Uid uid = marshaller.storeRootDir(newDirHandle);

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

    for (vector<Child*>::const_iterator it = children->begin(); it != children->end(); it++) { 
	Child *child = *it;
	cat.debug(" child %p", child);
	cat.debug(" name %s", child->getName().c_str());
    }

    return ChildIterator(children);
}

void CachedRootDirState::addFile(File& file)
    throw()
{
    RootDirState& newState =
	*new DirtyRootDirState(rootDir, uid, dirHandle, children);

    rootDir.setState(newState);
    newState.addFile(file);
}

void CachedRootDirState::removeFile(File& file)
    throw()
{
    RootDirState& newState =
	*new DirtyRootDirState(rootDir, uid, dirHandle, children);

    rootDir.setState(newState);
    newState.removeFile(file);
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

void DirtyRootDirState::addFile(File& file)
    throw()
{
    children->push_back(&file);
}

void DirtyRootDirState::removeFile(File& file)
    throw(IoError)
{
    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	File& f = dynamic_cast<File&>(**it);

	if (f.getName() == file.getName()) {
	    children->erase(it);
	    return;
	}
    }

    throw IoError(ENOENT, "File %s doesn't exist", file.getName().c_str());
}

void DirtyRootDirState::setDirty()
    throw() { }

void DirtyRootDirState::release()
    throw(IoError)
{
    FsMarshaller& marshaller = rootDir.getMarshaller();
    DirHandle newDirHandle = marshaller.newDirHandle();

    for (vector<Child*>::iterator it = children->begin(); it != children->end(); it++) {
	File& file = dynamic_cast<File&>(**it);

	file.addToDirHandle(newDirHandle);
    }

    Uid uid = marshaller.storeRootDir(newDirHandle);

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

RootDir::RootDir(FsMarshaller& marshaller)
    throw()
    : marshaller(marshaller), state(new NewRootDirState(*this)),
      attr(NodeAttrImpl(*this, *new ParentAttrPersistStrategy()))
{
    getAttr().setMode(S_IFDIR | S_IRWXU | S_IRGRP|S_IXGRP | S_IROTH|S_IXOTH);
}

RootDir::RootDir(FsMarshaller& marshaller, const Uid uid)
    throw(IoError)
    : marshaller(marshaller), state(new VirtualRootDirState(*this, uid)),
      attr(NodeAttrImpl(*this, *new ParentAttrPersistStrategy()))
{ }

RootDir::~RootDir()
    throw()
{
    delete &state;
}

// TODO Remove: debugging only
RootDir::RootDir(RootDir& rootDir)
    throw()
    : marshaller(rootDir.marshaller), state(rootDir.state),
      attr(NodeAttrImpl(*this, *new ParentAttrPersistStrategy()))
{
    int *p = NULL;

    *p = 0;
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
    attr.setMode((attr.getMode() | S_IFDIR) & ~S_IFREG);

    return attr;
}

FsMarshaller& RootDir::getMarshaller() const
    throw()
{
    return marshaller;
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
//    cat.debug("state = %s, getting childiterator", typeid(state).name());
    cat.debug(__PRETTY_FUNCTION__);
    cat.debug("&state = %p", &state);

    return state->childIterator();
}

void RootDir::addFile(File& file)
    throw(IoError)
{
    state->addFile(file);
}

void RootDir::removeFile(File& file)
    throw(IoError)
{
    state->removeFile(file);
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

// Dir::Dir(Dir* dir, string name, const Uid uid)
//     : File(dir, name), uid(uid), dirHandle(0), children(vector<Child*>(0))
// {
//     if (cat.isDebugEnabled()) {
// 	cat.debug("Nodes for directory <%s>:", name.c_str());
// 	for (vector<Node*>::iterator it = nodes.begin(); it != nodes.end(); it++)
// 	    cat.debug("\t<%s>", (*it)->getName().c_str());
//     }
// }

// const DirHandle Dir::getDirHandle() const { return dirHandle; }

// void Dir::setDirHandle(DirHandle _dirHandle) { dirHandle = _dirHandle; }

// const Uid Dir::getUid() const { return uid; }

// void Dir::setUid(const Uid _uid) { uid = _uid; }

// void Dir::addFile(File *file) { nodes.push_back(file); }

// vector<Node*>* Dir::getNodes() { return &nodes; }

log4cpp::Category& DirMarshallerException::cat = log4cpp::Category::getInstance("DirMarshallerException");

DirMarshallerException::DirMarshallerException(string msg, ...) : msg(msg)
{
    va_list ap;

    va_start(ap, msg);
    cat.logva(log4cpp::Priority::ERROR, msg.c_str(), ap);
    va_end(ap);
}

string DirMarshallerException::getMsg() const { return msg; }
