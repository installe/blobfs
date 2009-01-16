#ifndef _NODEATTRIMPL_HH_
#define _NODEATTRIMPL_HH_

#include "IoError.hh"
#include "Node.hh"
#include "NodeAttr.hh"
#include "NodeAttrPersistStrategy.hh"

#include <log4cpp/Category.hh>

class NodeAttrImpl : public NodeAttr {

private:
    static log4cpp::Category& cat;

    struct stat attr;

    Node& node;
    NodeAttrPersistStrategy& strategy;
    mode_t type;

public:
    NodeAttrImpl(Node& node, NodeAttrPersistStrategy& strategy, mode_t nodeType)
	throw();

    void setAttr(const struct stat& stbuf)
	throw();

    void getAttr(struct stat& stbuf) const
	throw();

    void setSize(off_t size)
	throw();

    off_t getSize()
	throw();

    void setMode(mode_t mode)
	throw();

    mode_t getMode() const
	throw();

    void setUid(uid_t uid)
	throw();

    void setGid(gid_t gid)
	throw();

    uid_t getUid() const
	throw();

    gid_t getGid() const
	throw();

    void setATime(time_t time)
	throw();

    void setMTime(time_t time)
	throw();

    void setCTime(time_t time)
	throw();

    time_t getATime() const
	throw();

    time_t getMTime() const
	throw();

    time_t getCTime() const
	throw();

    void persist() const
	throw(IoError);
};

#endif /* _NODEATTRIMPL_HH_ */
