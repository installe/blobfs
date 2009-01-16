#ifndef _PARENTMOCK_HH_
#define _PARENTMOCK_HH_

#include "ChildAttrPersistStrategy.hh"
#include "ChildIterator.hh"
#include "IoError.hh"
#include "NodeAttrImpl.hh"
#include "Parent.hh"

class ParentMock : public Parent {

private:
    NodeAttrImpl attr;
    ChildVector vec;

public:
    ParentMock()
	throw()
	: attr(*this, *new ChildAttrPersistStrategy()) { }

    NodeAttr& getAttr()
	throw() { return attr; }

    const NodeAttr& getAttr() const
	throw() { return attr; }

    const Node& getOwner() const
	throw() { return *this; }

    ChildIterator childIterator()
	throw(IoError) { return ChildIterator(vec); }

    void release()
	throw(IoError) { }

    void addChild(Child& child)
	throw(IoError) { }

    void removeChild(Child& child)
	throw(IoError) { }

    void setDirty()
	throw(IoError) { }

};

#endif /* _PARENTMOCK_HH_ */
