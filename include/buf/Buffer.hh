#ifndef _BUF_BUFFER_HH_
#define _BUF_BUFFER_HH_

#include <buf/BufferError.hh>
#include <buf/RangeError.hh>

namespace buf {

    class Buffer {

    public:
	/**
	 * @param idx index into the buffer.
	 * @returns reference to the byte at location idx into the buffer.
	 * @throws BufferError when the location has to be re-read, but
	 *  re-reading fails for some reason.
	 * @throws RangeError when the index is invalid.
	 */
	virtual char& operator[](size_t idx)
	    throw(BufferError, RangeError) = 0;

	virtual void setDirty()
	    throw() = 0;

	virtual ~Buffer()
	    throw() = 0;

    };

}

#endif /* _BUF_BUFFER_HH_ */
