#ifndef _BUF_RANGEERROR_HH_
#define _BUF_RANGEERROR_HH_

#include <string>

namespace buf {

    class RangeError {

    private:
	const std::string msg;

    public:
	RangeError(const std::string msg, ...)
	    throw();

	~RangeError()
	    throw();

	const char *what() const
	    throw();

    };

};

#endif
