#ifndef _PARENT_HH_
#define _PARENT_HH_

#include "ChildIterator.hh"
#include "Node.hh"

class Parent : public Node {

public:
    virtual ChildIterator childIterator()
	throw(IoError) = 0;

    // TODO Move to Node interface?
    virtual void release()
	throw(IoError) = 0;

    virtual void removeFile(File& file)
	throw(IoError) = 0;

    virtual void setDirty()
	throw(IoError) = 0;

};

#endif /* _PARENT_HH_ */
