#ifndef _BUF_OUTOFMEMORYERROR_HH_
#define _BUF_OUTOFMEMORYERROR_HH_

#include <string>

namespace buf {

    class OutOfMemoryError {

    private:
	const std::string msg;

    public:
	OutOfMemoryError(const std::string msg, ...)
	    throw();

	~OutOfMemoryError()
	    throw();

	const char *what() const
	    throw();

    };

};

#endif /* _BUF_OUTOFMEMORYERROR_HH_ */
