#include "filehandle.hh"
#include "InvalidArgumentError.hh"

class Child;
class FsMarshaller;
class Node;
class Parent;

enum NodeTypeEnum {
    DIR_TYPE = 2 << 0,
    FILE_TYPE = 2 << 1,
};

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

class ChildFactory {

public:
    // TODO Change FileHandle in signature to NodeHandle
    Child& newChild(FsMarshaller& marshaller, Parent& parent, FileHandle handle)
	throw(InvalidArgumentError);

};
