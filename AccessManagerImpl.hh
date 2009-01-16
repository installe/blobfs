#ifndef _ACCESSMANAGERIMPL_HH_
#define _ACCESSMANAGERIMPL_HH_

#include "AccessManager.hh"

class AccessManagerImpl : public AccessManager {

private:
    bool isMaskSet(const Node& node, mode_t mask) const
	throw();

    bool accessible(const Node& node, const UserContext context,
		    mode_t uMask, mode_t gMask, mode_t oMask) const
	throw();

public:
    void checkExecutable(const Node& node, const string path,
			 const UserContext context) const
	throw(IoError);

    void checkReadable(const Node& node, const string path,
		       const UserContext context) const
	throw(IoError);

    void checkWritable(const Node& node, const string path,
		       const UserContext context) const
	throw(IoError);

    void checkModifiable(const Node& node, const string path,
			 const UserContext context) const
	throw(IoError);

};

#endif /* _ACCESSMANAGERIMPL_HH_ */
