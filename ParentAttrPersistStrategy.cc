#include "file.hh"
#include "Parent.hh"
#include "ParentAttrPersistStrategy.hh"

void ParentAttrPersistStrategy::persist(Node& node)
    throw()
{
    Parent& parent = dynamic_cast<Parent&>(node);

    parent.setDirty();
    parent.release();
}
