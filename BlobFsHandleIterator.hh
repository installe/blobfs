#ifndef _BLOBFSHANDLEITERATOR_HH_
#define _BLOBFSHANDLEITERATOR_HH_

#include "HandleIterator.hh"
#include "IoError.hh"

#include <FPAPI.h>
#include <log4cpp/Category.hh>

class BlobFsHandleIterator : public HandleIterator {

private:
    static log4cpp::Category& cat;

    FPClipRef clipRef;
    FPTagRef nextTag;

    void checkForErrors(string action) const
	throw(IoError);

    void prefetchNext()
	throw(IoError);

    BlobFsHandleIterator(BlobFsHandleIterator& handleIterator)
	throw();

    BlobFsHandleIterator& operator=(const BlobFsHandleIterator& handleIterator)
	throw();

public:
    BlobFsHandleIterator(FPClipRef clipRef)
	throw(IoError);

    ~BlobFsHandleIterator()
    throw();

    bool hasNext()
	throw(IoError);

    FileHandle next()
	throw(IoError);

};

#endif /* _BLOBFSHANDLEITERATOR_HH_ */
