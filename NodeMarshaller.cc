#include "NodeMarshaller.hh"
#include "Dir.hh"
#include "file.hh"
#include "fsmarshaller.hh"
#include "InvalidArgumentError.hh"
#include "NodeAttrMarshaller.hh"
#include "RootDirMarshaller.hh"

#include <string>

using namespace std;

class NodeType {

private:
    NodeTypeEnum type;

public: 
    NodeType(int type)
	throw()
	: type(static_cast<NodeTypeEnum>(type)) { }

    operator int()
	throw() { return type; }

    int toInt()
	throw() { return type; }

};

NodeMarshaller::NodeMarshaller(FsMarshaller& marshaller,
			       RootDirMarshaller& rootDirMarshaller)
    throw()
    : marshaller(marshaller), rootDirMarshaller(rootDirMarshaller) { }

Child& NodeMarshaller::unmarshal(Parent& parent, FileHandle handle) const
    throw(IoError, InvalidArgumentError)
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

Uid NodeMarshaller::marshal(DirHandle dirHandle, RootDir& rootDir) const
    throw(IoError)
{
    for (ChildIterator it = rootDir.childIterator(); it.hasNext(); /* */) {
	Child& child = it.next();

	child.addToDirHandle(dirHandle);
    }

    Uid uid = marshaller.storeRootDir(dirHandle);

    rootDirMarshaller.marshal(rootDir, uid);
    return uid;
}
