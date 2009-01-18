#include "NodeMarshaller.hh"
#include "Dir.hh"
#include "NodeAttrMarshaller.hh"
#include "ParentAttrPersistStrategy.hh"

#include <log4cpp/Category.hh>

typedef vector<Child *>::iterator CVI;

class VirtualDirState : public DirState {

private:
    static log4cpp::Category& cat;

    Dir& dir;
    Uid uid;

    ChildVector listChildren(DirHandle dirHandle) const
	throw(IoError);

public:
    VirtualDirState(Dir& dir, const Uid uid)
	throw(IoError);

    ~VirtualDirState()
	throw();

    ChildIterator childIterator()
	throw(IoError);

    void addChild(Child& child)
	throw(IoError);

    void removeChild(Child& child)
	throw(IoError);

    void setDirty()
	throw(IoError);

    FileHandle addToDirHandle(DirHandle dirHandle)
	throw(IoError);

    void release()
	throw();

};

class NewDirState : public DirState {

private:
    static log4cpp::Category& cat;

    Dir& dir;
    ChildVector children;

public:
    NewDirState(Dir& dir)
	throw();

    ~NewDirState()
	throw();

    ChildIterator childIterator()
	throw();

    void addChild(Child& child)
	throw();

    void removeChild(Child& child)
	throw();

    void setDirty()
	throw();

    FileHandle addToDirHandle(DirHandle dirHandle)
	throw(IoError);

    void release()
	throw(IoError);

};

class CachedDirState : public DirState {

private:
    static log4cpp::Category& cat;

    Dir& dir;
    Uid uid;
    DirHandle dirHandle;
    ChildVector children;

public:
    CachedDirState(Dir& dir, const Uid uid, DirHandle dirHandle,
		   ChildVector children)
	throw();

    ~CachedDirState()
	throw();

    ChildIterator childIterator()
	throw();

    void addChild(Child& child)
	throw();

    void removeChild(Child& child)
	throw();

    void setDirty()
	throw();

    FileHandle addToDirHandle(DirHandle dirHandle)
	throw(IoError);

    void release()
	throw();

};

class DirtyDirState : public DirState {

private:
    static log4cpp::Category& cat;

    Dir& dir;
    Uid uid;
    DirHandle dirHandle;
    ChildVector children;

public:
    DirtyDirState(Dir& dir, const Uid uid, DirHandle dirHandle,
		  ChildVector children)
	throw();

    ~DirtyDirState()
	throw();

    ChildIterator childIterator()
	throw();

    void addChild(Child& child)
	throw();

    void removeChild(Child& child)
	throw(IoError);

    void setDirty()
	throw();

    FileHandle addToDirHandle(DirHandle dirHandle)
	throw(IoError);

    void release()
	throw(IoError);

};

// VirtualDirState

log4cpp::Category& VirtualDirState::cat =
    log4cpp::Category::getInstance("VirtualDirState");

VirtualDirState::VirtualDirState(Dir& dir, const Uid uid)
    throw(IoError)
    : dir(dir), uid(uid)
{
    cat.debug("VirtualDirState::VirtualDirState(%s)", uid.getClipID());
}

VirtualDirState::~VirtualDirState()
    throw()
{
    cat.debug("VirtualDirState::~VirtualDirState()");
}

ChildVector VirtualDirState::listChildren(DirHandle dirHandle) const
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    NodeHandleIterator it = marshaller.getHandleIterator(dirHandle);
    ChildVector children(new vector<Child *>);

    while (it.hasNext()) {
	// TODO Rename FileHandle to ChildHandle
	FileHandle handle = it.next();
	NodeMarshaller factory;
	Child& child = factory.newChild(marshaller, dir, handle);

	children->push_back(&child);
    }

    return children;
}

ChildIterator VirtualDirState::childIterator()
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    dir.setState(*new CachedDirState(dir, uid, dirHandle, children));

    return dir.childIterator();
}

void VirtualDirState::addChild(Child& child)
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    children->push_back(&child);
    dir.setState(*new DirtyDirState(dir, uid, dirHandle, children));
}

void VirtualDirState::removeChild(Child& child)
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    for (CVI it = children->begin(); it != children->end(); it++) {
	Child& c = **it;

	if (c.getName() == child.getName()) {
	    children->erase(it);
	    dir.setState(* new DirtyDirState(dir, uid, dirHandle, children));

	    return;
	}
    }

    dir.setState(*new CachedDirState(dir, uid, dirHandle, children));
    throw IoError(ENOENT, "Path <%s> doesn't exist", child.getName().c_str());
}

void VirtualDirState::setDirty()
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    DirHandle dirHandle = marshaller.openDirHandle(uid);
    ChildVector children = listChildren(dirHandle);

    dir.setState(*new DirtyDirState(dir, uid, dirHandle, children));
}

FileHandle VirtualDirState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    FileHandle newHandle = marshaller.addSubdir(dirHandle, uid);

    // TODO Include setting name in marshallDir()? Don't think so.
    marshaller.setStringAttr(newHandle, "name", dir.getName());
    attrMarshaller.marshallDir(dir.getAttr(), newHandle);
    return newHandle;
}

void VirtualDirState::release()
    throw() { }

// NewDirState

log4cpp::Category& NewDirState::cat =
    log4cpp::Category::getInstance("NewDirState");

NewDirState::NewDirState(Dir& dir)
    throw()
    : dir(dir), children(new vector<Child *>)
{
    cat.debug("NewDirState::NewDirState()");
}

NewDirState::~NewDirState()
    throw()
{
    cat.debug("NewDirState::~NewDirState()");
}

ChildIterator NewDirState::childIterator()
    throw()
{
    return ChildIterator(children);
}

void NewDirState::addChild(Child& child)
    throw()
{
    children->push_back(&child);
}

void NewDirState::removeChild(Child& child)
    throw()
{
    for (CVI it = children->begin(); it != children->end(); it++) {
	Child& c = **it;

	if (c.getName() == child.getName()) {
	    children->erase(it);
	    return;
	}
    }

    throw IoError(ENOENT, "Path <%s> doesn't exist", child.getName().c_str());
}

void NewDirState::setDirty()
    throw() { }

FileHandle NewDirState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    release();

    return dir.addToDirHandle(dirHandle);
}

void NewDirState::release()
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    DirHandle newDirHandle = marshaller.newDirHandle();

    // TODO seems like we only need to iterate over children in RootDir/Dir.
    // So remove childIterator() from the (public) interface.
    for (CVI it = children->begin(); it != children->end(); it++) {
	Child& child = **it;

	child.addToDirHandle(newDirHandle);
    }

    Uid uid = marshaller.storeDir(newDirHandle);

    cat.debug("Switching from new to cached; children -> %p", children.get());
    dir.setState(*new CachedDirState(dir, uid, newDirHandle, children));

    dir.getParent().setDirty();
    dir.getParent().release();
}

// CachedDirState

log4cpp::Category& CachedDirState::cat =
    log4cpp::Category::getInstance("CachedDirState");

CachedDirState::CachedDirState(Dir& dir, const Uid uid, DirHandle dirHandle,
			       ChildVector children)
    throw()
    : dir(dir), uid(uid), dirHandle(dirHandle), children(children)
{
    cat.debug("CachedDirState::CachedDirState(%s)", uid.getClipID());
}

CachedDirState::~CachedDirState()
    throw()
{
    cat.debug("CachedDirState::~CachedDirState()");
}

ChildIterator CachedDirState::childIterator()
    throw()
{
    return ChildIterator(children);
}

void CachedDirState::addChild(Child& child)
    throw()
{
    DirState& newState = *new DirtyDirState(dir, uid, dirHandle, children);

    dir.setState(newState);
    dir.addChild(child);
}

void CachedDirState::removeChild(Child& child)
    throw()
{
    DirState& newState = *new DirtyDirState(dir, uid, dirHandle, children);

    dir.setState(newState);
    dir.removeChild(child);
}

void CachedDirState::setDirty()
    throw()
{
    dir.setState(*new DirtyDirState(dir, uid, dirHandle, children));
}

FileHandle CachedDirState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    // TODO TBI
    FsMarshaller& marshaller = dir.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    FileHandle newHandle = marshaller.addSubdir(dirHandle, uid);

    // TODO Move logic below to NodeMarshaller? Same for all
    // other DirStates, FileStates, RootDirStates.
    // TODO Include setting name in marshallDir()? Don't think so.
    marshaller.setStringAttr(newHandle, "name", dir.getName());
    attrMarshaller.marshallDir(dir.getAttr(), newHandle);
    return newHandle;
}

void CachedDirState::release()
    throw() { }

// DirtyDirState

log4cpp::Category& DirtyDirState::cat =
    log4cpp::Category::getInstance("DirtyDirState");

DirtyDirState::DirtyDirState(Dir& dir, const Uid uid, DirHandle dirHandle,
			     ChildVector children)
    throw()
    : dir(dir), uid(uid), dirHandle(dirHandle), children(children)
{
    cat.debug("DirtyDirState::DirtyDirState(%s)", uid.getClipID());
}

DirtyDirState::~DirtyDirState()
    throw()
{
    cat.debug("DirtyDirState::~DirtyDirState(%s)", uid.getClipID());
}

ChildIterator DirtyDirState::childIterator()
    throw()
{
    return ChildIterator(children);
}

void DirtyDirState::addChild(Child& child)
    throw()
{
    children->push_back(&child);
}

void DirtyDirState::removeChild(Child& child)
    throw(IoError)
{
    for (CVI it = children->begin(); it != children->end(); it++) {
	Child& c = **it;

	if (c.getName() == child.getName()) {
	    children->erase(it);
	    delete &c;
	    return;
	}
    }

    throw IoError(ENOENT, "Path <%s> doesn't exist", child.getName().c_str());
}

void DirtyDirState::setDirty()
    throw() { }

FileHandle DirtyDirState::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    release();

    return dir.addToDirHandle(dirHandle);
}

void DirtyDirState::release()
    throw(IoError)
{
    FsMarshaller& marshaller = dir.getMarshaller();
    NodeAttrMarshaller attrMarshaller(marshaller);
    DirHandle newDirHandle = marshaller.newDirHandle();

    for (CVI it = children->begin(); it != children->end(); it++) {
	Child& child = **it;

	child.addToDirHandle(newDirHandle);
    }

    Uid uid = marshaller.storeDir(newDirHandle);

    marshaller.closeDirHandle(dirHandle);
    dir.setState(*new CachedDirState(dir, uid, newDirHandle, children));

    dir.getParent().setDirty();
    dir.getParent().release();
}

// Dir

log4cpp::Category& Dir::cat = log4cpp::Category::getInstance("Dir");

Dir::Dir(FsMarshaller& marshaller, Parent& parent, string name)
    throw()
    : Child(parent, name, S_IFDIR), marshaller(marshaller), parent(parent),
      attr(* new NodeAttrImpl(*this, *new ParentAttrPersistStrategy(), S_IFDIR)),
      state(new NewDirState(*this))
{
    cat.debug("Dir::Dir(%s)", getName().c_str());
}

Dir::Dir(FsMarshaller& marshaller, Parent& parent, string name, Uid uid)
    throw()
    : Child(parent, name, S_IFDIR), marshaller(marshaller), parent(parent),
      attr(* new NodeAttrImpl(*this, *new ParentAttrPersistStrategy(), S_IFDIR)),
      state(new VirtualDirState(*this, uid))
{
    cat.debug("Dir::Dir(%s, %s)", getName().c_str(), uid.getClipID());
    getAttr().setMode(S_IFDIR);
}

Dir::~Dir()
    throw()
{
    delete state;

    cat.debug("Dir::~Dir()", getName().c_str());
}

FsMarshaller& Dir::getMarshaller() const
    throw()
{
    return marshaller;
}

const DirState& Dir::getState() const
    throw()
{
    return *state;
}

void Dir::setState(DirState& state)
    throw()
{
    this->state = &state;
}

// TODO Refactor childIterator and other methods that delegate to state to Parent
ChildIterator Dir::childIterator()
    throw(IoError)
{
    return state->childIterator();
}

void Dir::addChild(Child& child)
    throw(IoError)
{
    state->addChild(child);
    parent.setDirty();
}

void Dir::removeChild(Child& child)
    throw(IoError)
{
    state->removeChild(child);
    parent.setDirty();
}

void Dir::setDirty()
    throw(IoError)
{
    state->setDirty();
    parent.setDirty();
}

FileHandle Dir::addToDirHandle(DirHandle dirHandle)
    throw(IoError)
{
    return state->addToDirHandle(dirHandle);
}

void Dir::release()
    throw(IoError)
{
    state->release();
}

// TODO Shouldn't the methods below be inherited from Child?

NodeAttr& Dir::getAttr()
    throw()
{
    return attr;
}

const NodeAttr& Dir::getAttr() const
    throw()
{
    return attr;
}

const Node& Dir::getOwner() const
    throw()
{
    return parent;
}
