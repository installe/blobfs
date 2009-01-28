#ifndef _BUF_IMPL_BUFFERMANAGERIMPL_HH_
#define _BUF_IMPL_BUFFERMANAGERIMPL_HH_

#include <buf/BufferManager.hh>

namespace buf {

    namespace impl {

	class BufferManagerImpl : public BufferManager {

	private:

	public:
	    Buffer& allocate(BufferFiller& buf, size_t size)
		throw(OutOfMemoryError);

	    void resize(Buffer& buf, size_t size)
		throw(OutOfMemoryError);

	};

    }

}

#endif /* _BUF_IMPL_BUFFERMANAGERIMPL_HH_ */
