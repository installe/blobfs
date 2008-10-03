#include "Child.hh"
#include "ChildAttrPersistStrategy.hh"
#include "file.hh" // TODO This should move when file.hh is split up in parts
#include "Parent.hh"

void ChildAttrPersistStrategy::persist(Node& node)
    throw()
{
    Child& child = dynamic_cast<Child&>(node);
    Parent& parent = child.getParent();

    parent.setDirty();
    parent.release();
}
