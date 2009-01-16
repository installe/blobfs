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

    void marshallNode(const NodeAttr& attr, const FileHandle fHandle) const
	throw(IoError);

    void unmarshallNode(NodeAttr& attr, const FileHandle fHandle) const
	throw(IoError);

public:
    // TODO make FsMarshaller const
    NodeAttrMarshaller(FsMarshaller& marshaller)
	throw();

    void marshallFile(const NodeAttr& attr, const FileHandle fHandle) const
	throw(IoError);

    void unmarshallFile(NodeAttr& attr, const FileHandle fHandle) const
	throw(IoError);

    void marshallDir(const NodeAttr& attr, const FileHandle fHandle) const
	throw(IoError);

    void unmarshallDir(NodeAttr& attr, const FileHandle fHandle) const
	throw(IoError);
};

#endif /* _NODEATTRMARSHALLER_HH_ */
