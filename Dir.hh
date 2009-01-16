#include "Child.hh"
#include "fsmarshaller.hh"
#include "Parent.hh"

#include <string>

using namespace std;

class DirState {

public:
    virtual ChildIterator childIterator()
	throw(IoError) = 0;

    virtual void addChild(Child& child)
	throw(IoError) = 0;

    virtual void removeChild(Child& child)
	throw(IoError) = 0;

    virtual void setDirty()
	throw(IoError) = 0;

    virtual void release()
	throw(IoError) = 0;

    virtual FileHandle addToDirHandle(DirHandle dirHandle)
	throw(IoError) = 0;

    virtual ~DirState()
	throw() { }

};

class Dir : public virtual Parent, public virtual Child {

private:
    static log4cpp::Category& cat;

    FsMarshaller& marshaller;
    Parent& parent;
    string name;
    NodeAttr& attr;
    DirState *state;

public:
    // Ctors

    Dir(FsMarshaller& marshaller, Parent& parent, string name)
	throw();

    Dir(FsMarshaller& marshaller, Parent& parent, string name, Uid uid)
	throw();

    ~Dir()
	throw();

    // Introspection

    FsMarshaller& getMarshaller() const
	throw();

    NodeAttr& getAttr()
	throw();

    const NodeAttr& getAttr() const
	throw();

    const Node& getOwner() const
	throw();

    const DirState& getState() const
	throw();

    void setState(DirState& state)
	throw();

    // Operations

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
	throw(IoError);

};
