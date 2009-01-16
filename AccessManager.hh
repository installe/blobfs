#ifndef _ACCESSMANAGER_HH_
#define _ACCESSMANAGER_HH_

#include "IoError.hh"
#include "UserContext.hh"

class Node;

// TODO Maybe change all methods to take a NodeAttr& argument iso Node&?
class AccessManager {

// TODO Remove?
// private:
//     virtual bool isReadable(const Node& node, const string path,
// 			    const UserContext context) const
// 	throw() = 0;

//     virtual bool isWritable(const Node& node, const string path,
// 			    const UserContext context) const
// 	throw() = 0;

public:
    virtual void checkReadable(const Node& node, const string path,
			       const UserContext context) const
	throw(IoError) = 0;

    virtual void checkWritable(const Node& node, const string path,
			       const UserContext context) const
	throw(IoError) = 0;

    virtual void checkExecutable(const Node& node, const string path,
				 const UserContext context) const
	throw(IoError) = 0;

    virtual void checkModifiable(const Node& node, const string path,
				 const UserContext context) const
	throw(IoError) = 0;

};

#endif /* _ACCESSMANAGER_HH_ */
