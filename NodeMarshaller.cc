#include "NodeMarshaller.hh"
#include "Dir.hh"
#include "file.hh"
#include "fsmarshaller.hh"
#include "InvalidArgumentError.hh"
#include "NodeAttrMarshaller.hh"

#include <string>

using namespace std;

Child& NodeMarshaller::newChild(FsMarshaller& marshaller, Parent& parent,
				FileHandle handle)
    throw(InvalidArgumentError)
{
    string name = marshaller.getStringAttr(handle, "name");
    NodeType type = marshaller.getLongAttr(handle, "type");
    NodeAttrMarshaller attrMarshaller(marshaller);
    Child *child = NULL;

    if (type == FILE_TYPE) {
	child = new File(marshaller, parent, name, handle);
	attrMarshaller.unmarshallFile(child->getAttr(), handle);
    } else if (type == DIR_TYPE) {
	Uid uid(marshaller.getStringAttr(handle, "clipid").c_str());

	child = new Dir(marshaller, parent, name, uid);
	attrMarshaller.unmarshallDir(child->getAttr(), handle);
    } else {
	throw InvalidArgumentError("Node <%s> has invalid node type %i",
				   name.c_str(), type.toInt());
    }

    return *child;
}
