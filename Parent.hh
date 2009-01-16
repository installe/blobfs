#ifndef _PARENT_HH_
#define _PARENT_HH_

#include "ChildIterator.hh"
#include "LockGuard.hh"
#include "Node.hh"

#include <boost/thread/mutex.hpp>

using boost::mutex;

class Parent : public virtual Node {

private:
    mutex m;

public:
    virtual ~Parent()
	throw() { }

    virtual ChildIterator childIterator()
	throw(IoError) = 0;

    // TODO Move to Node interface?
    virtual void release()
	throw(IoError) = 0;

    virtual void addChild(Child& child)
	throw(IoError) = 0;

    // TODO Should be removeChild(Child& child). NO: Dirs can't be removed,
    // and are also children.
    virtual void removeChild(Child& child)
	throw(IoError) = 0;

    virtual void setDirty()
	throw(IoError) = 0;

    virtual LockGuard<> guard()
	throw();

};

#endif /* _PARENT_HH_ */
