#ifndef _CHILDATTRPERSISTSTRATEGY_HH_
#define _CHILDATTRPERSISTSTRATEGY_HH_

#include "NodeAttrPersistStrategy.hh"

class ChildAttrPersistStrategy : public NodeAttrPersistStrategy {

public:
    void persist(Node& node)
	throw();

};

#endif /* _CHILDATTRPERSISTSTRATEGY_HH_ */
