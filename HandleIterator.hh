#ifndef _HANDLEITERATOR_HH_
#define _HANDLEITERATOR_HH_

#include "filehandle.hh"
#include "IoError.hh"

class HandleIterator {

public:
    virtual ~HandleIterator()
	throw() { };

    virtual bool hasNext()
	throw(IoError) = 0;

    virtual FileHandle next()
	throw(IoError) = 0;

};

#endif /* _HANDLEITERATOR_HH_ */
