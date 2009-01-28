#include "filehandle.hh"
#include "InvalidArgumentError.hh"
#include "IoError.hh"
#include "Uid.hh"

class Child;
class ChildIterator;
class Dir;
class FsMarshaller;
class Node;
class Parent;
class RootDir;
class RootDirMarshaller;

enum NodeTypeEnum {
    DIR_TYPE = 2 << 0,
    FILE_TYPE = 2 << 1,
};

class NodeMarshaller {

private:
    FsMarshaller& marshaller;
    RootDirMarshaller& rootDirMarshaller;

public:
    NodeMarshaller(FsMarshaller& marshaller,
		   RootDirMarshaller& rootDirMarshaller)
	throw();

    // TODO Change FileHandle in signature to NodeHandle (or ChildHandle?)
    Child& unmarshal(Parent& parent, FileHandle handle) const
	throw(IoError, InvalidArgumentError);

    Uid marshal(DirHandle dirHandle, RootDir& rootDir) const
	throw(IoError);

    Uid marshal(DirHandle dirHandle, Dir& dir) const
	throw(IoError);

};
