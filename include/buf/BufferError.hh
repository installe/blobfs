#ifndef _BUF_BUFFERERROR_HH_
#define _BUF_BUFFERERROR_HH_

#include <exception>
#include <string>

namespace buf {

    class BufferError : public std::exception {

    private:
	const std::string msg;

    public:
	BufferError(const std::string msg, ...)
	    throw();

	~BufferError()
	    throw();

	const char *what() const
	    throw();

    };

}

#endif /* _BUF_BUFFERERROR_HH_ */
