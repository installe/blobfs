#include "AccessManagerImpl.hh"
#include "Node.hh"
#include "NodeAttr.hh"

#include <errno.h>

#include <iostream>

bool AccessManagerImpl::isMaskSet(const Node& node, mode_t mask) const
    throw()
{
    return node.getAttr().getMode() & mask;
}

bool AccessManagerImpl::accessible(const Node& node, const UserContext context,
				   mode_t uMask, mode_t gMask, mode_t oMask)
    const
    throw()
{
    const NodeAttr& attr = node.getAttr();
    mode_t mode = attr.getMode();

    if (mode & oMask)
	return true;

    if ((mode & uMask) && context.getUid() == attr.getUid())
	return true;

    if ((mode & gMask) && context.getGid() == attr.getGid())
	return true;

    return false;
}

void AccessManagerImpl::checkReadable(const Node& node, const string path,
				      const UserContext context) const
    throw(IoError)
{
    if (accessible(node, context, S_IRUSR, S_IRGRP, S_IROTH))
	return;

    throw IoError(EPERM, "path <%s> is not readable", path.c_str());
}

void AccessManagerImpl::checkWritable(const Node& node, const string path,
				      const UserContext context) const
    throw(IoError)
{
    if (accessible(node, context, S_IWUSR, S_IWGRP, S_IWOTH))
	return;

    throw IoError(EPERM, "path <%s> is not writable", path.c_str());
}

void AccessManagerImpl::checkExecutable(const Node& node, const string path,
					const UserContext context) const
    throw(IoError)
{
    if (accessible(node, context, S_IXUSR, S_IXGRP, S_IXOTH))
	return;

    throw IoError(EPERM, "path <%s> is not executable", path.c_str());
}

void AccessManagerImpl::checkModifiable(const Node& node, const string path,
					const UserContext context) const
    throw(IoError)
{
    if (accessible(node.getOwner(), context, S_IWUSR, S_IWGRP, S_IWOTH)) {
	if (! isMaskSet(node, S_ISVTX))
	    return;

	if (node.getAttr().getUid() == context.getUid())
	    return;
    }

    throw IoError(EPERM, "path <%s> is not modifiable", path.c_str());
}
