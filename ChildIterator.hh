#ifndef _CHILDITERATOR_HH_
#define _CHILDITERATOR_HH_

#include "Child.hh"

#include <boost/shared_ptr.hpp>
#include <log4cpp/Category.hh>
#include <vector>

using namespace std;

typedef boost::shared_ptr<vector <Child *> > ChildVector;

class ChildIterator {

private:
    static log4cpp::Category& cat;

    ChildVector children;
    vector<Child*>::iterator it;
    vector<Child*>::iterator end;

public:
    ChildIterator(ChildVector children)
	throw();

    bool hasNext()
	throw();

    Child* next()
	throw();
    
};

#endif /* _CHILDITERATOR_HH_ */
