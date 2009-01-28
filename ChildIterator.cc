#include "ChildIterator.hh"

log4cpp::Category& ChildIterator::cat =
    log4cpp::Category::getInstance("ChildIterator");

ChildIterator::ChildIterator(ChildVector children)
    throw()
    : children(children), it(children->begin()), end(children->end())
{
    cat.debug("*children = %p", children.get());
}

bool ChildIterator::hasNext()
    throw()
{
    return it != end;
}

Child& ChildIterator::next()
    throw()
{
    return **it++;
}
