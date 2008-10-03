#include "NodeAttrMarshaller.hh"

log4cpp::Category& NodeAttrMarshaller::cat =
    log4cpp::Category::getInstance("NodeAttrMarshaller");

NodeAttrMarshaller::NodeAttrMarshaller(FsMarshaller& marshaller)
    throw()
    : marshaller(marshaller) { }

void NodeAttrMarshaller::marshallFile(const NodeAttr& attr, FileHandle fHandle)
    const
    throw(IoError)
{
    marshaller.setFileLongAttr(fHandle, "uid", attr.getUid());
    marshaller.setFileLongAttr(fHandle, "gid", attr.getGid());
    marshaller.setFileLongAttr(fHandle, "mode", attr.getMode());
}

void NodeAttrMarshaller::unmarshallFile(NodeAttr& attr, FileHandle fHandle)
    const
    throw(IoError)
{
    size_t size = marshaller.getFileSize(fHandle);

    cat.debug("got size: %zu", size);

    attr.setSize(size);
    attr.setUid(marshaller.getFileLongAttr(fHandle, "uid"));
    attr.setGid(marshaller.getFileLongAttr(fHandle, "gid"));
    attr.setMode(marshaller.getFileLongAttr(fHandle, "mode"));
}
