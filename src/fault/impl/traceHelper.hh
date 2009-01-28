#ifndef _FAULT_IMPL_TRACEHELPER_HH_
#define _FAULT_IMPL_TRACEHELPER_HH_

#include <string>
#include <vector>

namespace fault {

    namespace impl {

	const std::string demangle(const std::string name)
	    throw();

	const std::vector<std::string> trace()
	    throw();

    }

}

#endif /* _FAULT_IMPL_TRACEHELPER_HH_ */
