#ifndef _BUF_BUFFERMANAGER_HH_
#define _BUF_BUFFERMANAGER_HH_

#include <buf/OutOfMemoryError.hh>

#include <sys/types.h>

namespace buf {

    class Buffer;
    class BufferFiller;

    class BufferManager {

    public:
	virtual Buffer& allocate(BufferFiller& filler, size_t size)
	    throw(OutOfMemoryError) = 0;

	virtual void resize(Buffer& buf, size_t size)
	    throw(OutOfMemoryError) = 0;

	virtual ~BufferManager()
	    throw() { }

    };

}

#endif /* _BUF_BUFFERMANAGER_HH_ */
