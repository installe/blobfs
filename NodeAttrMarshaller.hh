#ifndef _NODEATTRMARSHALLER_HH_
#define _NODEATTRMARSHALLER_HH_

#include <log4cpp/Category.hh>

#include "filehandle.hh"
#include "fsmarshaller.hh"
#include "IoError.hh"
#include "NodeAttr.hh"

class NodeAttrMarshaller {

private:
    static log4cpp::Category& cat;

    FsMarshaller& marshaller;

public:
    NodeAttrMarshaller(FsMarshaller& marshaller)
	throw();

    void marshallFile(const NodeAttr& attr, FileHandle fHandle) const
	throw(IoError);

    void unmarshallFile(NodeAttr& attr, FileHandle fHandle) const
	throw(IoError);

    void marshallRootDir(const NodeAttr& attr, DirHandle dirHandle) const
	throw(IoError);

    void unmarshallRootDir(NodeAttr& attr, DirHandle dirHandle) const
	throw(IoError);
};

#endif /* _NODEATTRMARSHALLER_HH_ */
