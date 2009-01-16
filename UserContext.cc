#include "UserContext.hh"

UserContext::UserContext()
    throw()
    : context(fuse_get_context()) { }

const uid_t UserContext::getUid() const
    throw()
{
    return context->uid;
}

const gid_t UserContext::getGid() const
    throw()
{
    return context->gid;
}
