#ifndef _NODEATTR_HH_
#define _NODEATTR_HH_

#include "IoError.hh"

#include <sys/stat.h>

class NodeAttr {

public:
    virtual void getAttr(struct stat &stbuf) const
	throw() = 0;

    virtual void setSize(off_t size)
	throw() = 0;

    virtual off_t getSize()
	throw() = 0;

    virtual void setMode(mode_t mode)
	throw() = 0;

    virtual mode_t getMode() const
	throw() = 0;

    virtual void setUid(uid_t uid)
	throw() = 0;

    virtual void setGid(gid_t gid)
	throw() = 0;

    virtual uid_t getUid() const
	throw() = 0;

    virtual gid_t getGid() const
	throw() = 0;

    virtual void setATime(time_t time)
	throw() = 0;

    virtual void setMTime(time_t time)
	throw() = 0;

    virtual void setCTime(time_t time)
	throw() = 0;

    virtual time_t getATime() const
	throw() = 0;

    virtual time_t getMTime() const
	throw() = 0;

    virtual time_t getCTime() const
	throw() = 0;

    virtual void persist() const
	throw(IoError) = 0;
};

#endif /* _NODEATTR_HH_ */
