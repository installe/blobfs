#ifndef _CHILD_HH_
#define _CHILD_HH_

#include "Node.hh"
#include "NodeAttrImpl.hh"

#include <string>

using namespace std;

class Parent;

class Child : public Node {

private:
    NodeAttrImpl attr;

public:
    Child()
	throw();

    NodeAttr& getAttr()
	throw();

    virtual const string getName() const
	throw() = 0;

    virtual Parent& getParent() const
	throw() = 0;

    ~Child()
	throw() { };

};

#endif /* _CHILD_HH_ */
