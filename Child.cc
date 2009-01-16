#include "Child.hh"
#include "ChildAttrPersistStrategy.hh"
#include "file.hh"
#include "Parent.hh"

Child::Child(Parent& parent, string name, mode_t nodeType)
    throw()
    : parent(parent), name(name),
      attr(NodeAttrImpl(*this, *new ChildAttrPersistStrategy(), nodeType)) { }

const string Child::getName() const
    throw()
{
    return name;
}

const Parent& Child::getParent() const
    throw()
{
    return parent;
}

Parent& Child::getParent()
    throw()
{
    return parent;
}

const NodeAttr& Child::getAttr() const
    throw()
{
    return attr;
}

NodeAttr& Child::getAttr()
    throw()
{
    return attr;
}

const Node& Child::getOwner() const
    throw()
{
    return parent;
}
