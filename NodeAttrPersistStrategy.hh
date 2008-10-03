#ifndef _NODEATTRPERSISTSTRATEGY_HH_
#define _NODEATTRPERSISTSTRATEGY_HH_

#include "Node.hh"

class NodeAttrPersistStrategy {

public:
    virtual void persist(Node& node)
	throw() = 0;

    virtual ~NodeAttrPersistStrategy()
	throw() { };

};

#endif /* _NODEATTRPERSISTSTRATEGY_HH_ */
