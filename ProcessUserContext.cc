#include "ProcessUserContext.hh"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

ProcessUserContext::ProcessUserContext()
    throw()
    : uid(getuid()), gid(getgid()), mode(calcMode()) { }


const mode_t ProcessUserContext::calcMode() const
    throw()
{
    mode_t prevUmask = umask(0);

    umask(prevUmask);
    return ~S_IFMT & ~(prevUmask|S_ISVTX|S_ISUID|S_ISGID);
}

const uid_t ProcessUserContext::getUid() const
    throw() { return uid; }

const gid_t ProcessUserContext::getGid() const
    throw() { return gid; }

const mode_t ProcessUserContext::getMode() const
    throw() { return mode; }
