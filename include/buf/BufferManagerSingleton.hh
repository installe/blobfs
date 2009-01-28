#ifndef _BUF_BUFFERMANAGERSINGLETON_HH_
#define _BUF_BUFFERMANAGERSINGLETON_HH_

#include <boost/thread/mutex.hpp>

class BufferManager;

namespace buf {

    class BufferManagerSingleton {

    private:
	static boost::mutex lock;
	static BufferManager *instance;

    public:
	static BufferManager& getInstance()
	    throw();

    };

}

#endif /* _BUF_BUFFERMANAGERSINGLETON_HH_ */
