#include "Child.hh"
#include "ChildAttrPersistStrategy.hh"

Child::Child()
    throw()
    : attr(NodeAttrImpl(*this, *new ChildAttrPersistStrategy())) { }

NodeAttr& Child::getAttr()
    throw()
{
    return attr;
}
