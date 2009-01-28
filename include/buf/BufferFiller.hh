#ifndef _BUF_BUFFERFILLER_HH_
#define _BUF_BUFFERFILLER_HH_

#include <buf/BufferError.hh>
#include <buf/RangeError.hh>

namespace buf {

    class Buffer;

    class BufferFiller {

    public:
	virtual void fill(Buffer& buf, size_t start, size_t end)
	    throw(BufferError, RangeError) = 0;

	virtual ~BufferFiller()
	    throw() { }

    };

}

#endif /* _BUF_BUFFERFILLER_HH_ */
