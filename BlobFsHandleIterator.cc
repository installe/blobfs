#include "BlobFsHandleIterator.hh"

#include <errno.h>

log4cpp::Category& BlobFsHandleIterator::cat =
    log4cpp::Category::getInstance("BlobFsHandleIterator");

BlobFsHandleIterator::BlobFsHandleIterator(FPClipRef clipRef)
    throw(IoError)
    : clipRef(clipRef)
{
    prefetchNext();
}

BlobFsHandleIterator::~BlobFsHandleIterator()
    throw() { }

void BlobFsHandleIterator::checkForErrors(string action) const
    throw(IoError)
{
    FPErrorInfo errorInfo;

    FPPool_GetLastErrorInfo(&errorInfo);

    if (errorInfo.error) {
	cat.error("Error %i(%i): %s %s", errorInfo.error, errorInfo.errorClass,
		  errorInfo.errorString, errorInfo.message);
	cat.error(" systemError: %i", errorInfo.systemError);
	cat.error(errorInfo.trace);

	throw IoError(EIO, "Can't %s: %s", action.c_str(), errorInfo.message);
    }
}

bool BlobFsHandleIterator::hasNext()
    throw(IoError)
{
    return nextTag != 0;
}

FileHandle BlobFsHandleIterator::next()
    throw(IoError)
{
    FPTagRef currentTag = nextTag;

    prefetchNext();

    return currentTag;
}

void BlobFsHandleIterator::prefetchNext()
    throw(IoError)
{
    nextTag = FPClip_FetchNext(clipRef);

    cat.debug("clipRef %lli - fetched %lli", clipRef, nextTag);
    checkForErrors("fetch next FileHandle element");
}
