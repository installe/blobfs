#ifndef _PARENTATTRPERSISTSTRATEGY_HH_
#define _PARENTATTRPERSISTSTRATEGY_HH_

#include "NodeAttrPersistStrategy.hh"

class ParentAttrPersistStrategy : public NodeAttrPersistStrategy {

public:
    void persist(Node& node)
	throw();

};

#endif /* _PARENTATTRPERSISTSTRATEGY_HH_ */
