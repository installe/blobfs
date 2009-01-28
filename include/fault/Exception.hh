#ifndef _FAULT_EXCEPTION_HH_
#define _FAULT_EXCEPTION_HH_

#include <fault/Stacktrace.hh>

#include <exception>
#include <string>

namespace fault {

    class Exception : public std::exception {

    private:
	std::string msg;
	const Exception *reason;
	const Stacktrace trace;

    protected:
	const std::string message() const
	    throw();

	virtual const Exception *clone() const
	    throw() = 0;

    public:
	Exception(const Exception *reason, const char *msg, ...)
	    throw()
	    __attribute__ ((__format__ (__printf__, 3, 4)));

	Exception(const std::exception *reason, const char *msg, ...)
	    throw()
	    __attribute__ ((__format__ (__printf__, 3, 4)));

	Exception(const char *msg, ...)
	    throw()
	    __attribute__ ((__format__ (__printf__, 2, 3)));

	~Exception()
	    throw();

	const char *what() const
	    throw();

    };

}

#endif /* _FAULT_EXCEPTION_HH_ */
