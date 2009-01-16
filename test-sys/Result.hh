#ifndef _RESULT_HH_
#define _RESULT_HH_

#include <boost/lexical_cast.hpp>
#include <string>

class Result {

public:
    virtual void reportFailure(std::string file, int line, std::string msg)
	throw() = 0;

    virtual std::string toString() const
	throw() = 0;

};

void assertTrue(Result& result, bool b, const std::string msg = "");

template <class T> void assertEquals(Result& result, const T a, T b,
				     const std::string msg = "")
{
    if (a != b) {
	result.reportFailure("", -1, msg + ". Expected: "
			     + boost::lexical_cast<std::string>(a)
			     + ", Actual: "
			     + boost::lexical_cast<std::string>(b));
    }
}

#endif /* _RESULT_HH_ */
