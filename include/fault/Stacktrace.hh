#ifndef _FAULT_STACKTRACE_HH_
#define _FAULT_STACKTRACE_HH_

#include <ostream>
#include <string>
#include <vector>

namespace fault {

    class Stacktrace {

    private:
	const std::vector<std::string> stacktrace;
	size_t skip;

    public:
	Stacktrace(size_t skip = 0)
	    throw();

	void put(std::ostream& os) const
	    throw();

    };

    std::ostream& operator<<(std::ostream& os, const Stacktrace& trace)
	throw();

}

#endif /* _FAULT_STACKTRACE_HH_ */
