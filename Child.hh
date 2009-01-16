#ifndef _CHILD_HH_
#define _CHILD_HH_

#include "filehandle.hh"
#include "Node.hh"
#include "NodeAttrImpl.hh"

#include <string>

using namespace std;

class Parent;

class Child : public virtual Node {

private:
    Parent& parent;
    string name;
    NodeAttrImpl attr;

public:
    Child(Parent& parent, string name, mode_t nodeType)
	throw();

    const string getName() const
	throw();

    const Parent& getParent() const
	throw();

    Parent& getParent()
	throw();

    const NodeAttr& getAttr() const
	throw();

    NodeAttr& getAttr()
	throw();

    const Node& getOwner() const
	throw();

    virtual FileHandle addToDirHandle(DirHandle dirHandle)
	throw(IoError) = 0;

    ~Child()
	throw() { };

};

#endif /* _CHILD_HH_ */
