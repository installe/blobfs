#include "NodeAttrMarshaller.hh"

log4cpp::Category& NodeAttrMarshaller::cat =
    log4cpp::Category::getInstance("NodeAttrMarshaller");

NodeAttrMarshaller::NodeAttrMarshaller(FsMarshaller& marshaller)
    throw()
    : marshaller(marshaller) { }

void NodeAttrMarshaller::marshallNode(const NodeAttr& attr, const FileHandle fHandle)
    const
    throw(IoError)
{
    marshaller.setLongAttr(fHandle, "uid", attr.getUid());
    marshaller.setLongAttr(fHandle, "gid", attr.getGid());
    marshaller.setLongAttr(fHandle, "mode", attr.getMode());

    marshaller.setLongAttr(fHandle, "ctime", attr.getCTime());
    marshaller.setLongAttr(fHandle, "mtime", attr.getMTime());
    marshaller.setLongAttr(fHandle, "atime", attr.getATime());
}

void NodeAttrMarshaller::unmarshallNode(NodeAttr& attr, const FileHandle fHandle)
    const
    throw(IoError)
{
    attr.setUid(marshaller.getLongAttr(fHandle, "uid"));
    attr.setGid(marshaller.getLongAttr(fHandle, "gid"));
    attr.setMode(marshaller.getLongAttr(fHandle, "mode"));

    attr.setCTime(marshaller.getLongAttr(fHandle, "ctime"));
    attr.setMTime(marshaller.getLongAttr(fHandle, "mtime"));
    attr.setATime(marshaller.getLongAttr(fHandle, "atime"));
}

void NodeAttrMarshaller::marshallFile(const NodeAttr& attr, const FileHandle fHandle)
    const
    throw(IoError)
{
    marshallNode(attr, fHandle);
}

void NodeAttrMarshaller::unmarshallFile(NodeAttr& attr, FileHandle fHandle)
    const
    throw(IoError)
{
    size_t size = marshaller.getFileSize(fHandle);

    cat.debug("got size: %zu", size);

    attr.setSize(size);
    unmarshallNode(attr, fHandle);
}

void NodeAttrMarshaller::marshallDir(const NodeAttr& attr, const FileHandle fHandle)
    const
    throw(IoError)
{
    marshallNode(attr, fHandle);
}

void NodeAttrMarshaller::unmarshallDir(NodeAttr& attr, const FileHandle fHandle)
    const
    throw(IoError)
{
    unmarshallNode(attr, fHandle);
}
