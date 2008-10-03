#ifndef _NODE_HH_
#define _NODE_HH_

#include "NodeAttr.hh"

class Node {

public:
    virtual NodeAttr& getAttr()
	throw() = 0;

    virtual ~Node()
	throw() { };

};

#endif /* _NODE_HH_ */
