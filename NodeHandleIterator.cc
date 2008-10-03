#include "NodeHandleIterator.hh"

NodeHandleIterator::NodeHandleIterator(HandleIterator& handleIterator)
    throw(IoError)
    : it(&handleIterator) { }

NodeHandleIterator::NodeHandleIterator(const NodeHandleIterator& nodeHandleIterator)
    throw()
    : it(nodeHandleIterator.it) { }

NodeHandleIterator::~NodeHandleIterator()
    throw() { }

bool NodeHandleIterator::hasNext()
    throw(IoError)
{
    return it->hasNext();
}

FileHandle NodeHandleIterator::next()
    throw(IoError)
{
    return it->next();
}
