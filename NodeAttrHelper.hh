#ifndef _NODEATTRHELPER_HH_
#define _NODEATTRHELPER_HH_

#include "NodeAttr.hh"

class NodeAttrHelper {

public:
    void setPerms(NodeAttr& attr, mode_t mode) const
	throw()
    {
	mode_t orig = attr.getMode();

	attr.setMode((orig & S_IFMT) | (mode & ~S_IFMT));
    }

};

#endif /* _NODEATTRHELPER_HH_ */
