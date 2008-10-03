#include "NodeAttrImpl.hh"

#include <string.h>

NodeAttrImpl::NodeAttrImpl(Node& node, NodeAttrPersistStrategy& strategy)
    throw()
    : node(node), strategy(strategy)
{
    // TODO
    memset(&attr, 0, sizeof(attr));
}

void NodeAttrImpl::setAttr(const struct stat& stbuf)
    throw()
{
    attr = stbuf;
}

void NodeAttrImpl::getAttr(struct stat& stbuf) const
    throw()
{
    stbuf = attr;
}

void NodeAttrImpl::setSize(off_t size)
    throw() { attr.st_size = size; }

off_t NodeAttrImpl::getSize()
    throw() { return attr.st_size; }

void NodeAttrImpl::setMode(mode_t mode)
    throw() { attr.st_mode = mode; }

mode_t NodeAttrImpl::getMode() const
    throw() { return attr.st_mode; }

void NodeAttrImpl::setUid(uid_t uid)
    throw() { attr.st_uid = uid; }

void NodeAttrImpl::setGid(gid_t gid)
    throw() { attr.st_gid = gid; }

uid_t NodeAttrImpl::getUid() const
    throw() { return attr.st_uid; }

gid_t NodeAttrImpl::getGid() const
    throw() { return attr.st_gid; }

void NodeAttrImpl::setATime(time_t time)
    throw() { attr.st_atime = time; }

void NodeAttrImpl::setMTime(time_t time)
    throw() { attr.st_mtime = time; }

void NodeAttrImpl::setCTime(time_t time)
    throw() { attr.st_ctime = time; }

time_t NodeAttrImpl::getATime() const
    throw() { return attr.st_atime; }

time_t NodeAttrImpl::getMTime() const
    throw() { return attr.st_mtime; }

time_t NodeAttrImpl::getCTime() const
    throw() { return attr.st_ctime; }

void NodeAttrImpl::persist() const
    throw(IoError)
{
    // TODO
    strategy.persist(node);
}
