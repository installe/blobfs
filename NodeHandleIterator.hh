#ifndef _NODEHANDLEITERATOR_HH_
#define _NODEHANDLEITERATOR_HH_

#include "HandleIterator.hh"
#include "IoError.hh"

#include <boost/shared_ptr.hpp>

class NodeHandleIterator {

private:
    boost::shared_ptr<HandleIterator> it;

public:
    NodeHandleIterator(HandleIterator& handleIterator)
	throw(IoError);

    NodeHandleIterator(const NodeHandleIterator& nodeHandleIterator)
	throw();

    NodeHandleIterator& operator=(NodeHandleIterator& it)
	throw();

    ~NodeHandleIterator()
	throw();

    bool hasNext()
	throw(IoError);

    FileHandle next()
	throw(IoError);

};

#endif /* _NODEHANDLEITERATOR_HH_ */
