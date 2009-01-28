#include "traceHelper.hh"

#include <cxxabi.h>
#include <dlfcn.h>
#include <stdlib.h>

namespace fault {

    namespace impl {

	template<class T> class MallocGuard {

	private:
	    const T *t;

	public:
	    MallocGuard(T *t)
		throw()
		: t(t) { }

	    ~MallocGuard()
		throw()
	    {
		free(t);
	    }

	    T *get()
		throw()
	    {
		return t;
	    }

	};

	const std::string demangle(const std::string name)
	    throw()
	{
	    int status = 0;
	    MallocGuard guard(abi::__cxa_demangle(name, 0, 0, &status));

	    return guard->get() ? guard : name;
	}

	const std::vector<std::string> trace()
	    throw()
	{
	    std::vector<std::string> vec;
	    Dl_info info;
	    void **frame = static_cast<void **>(__builtin_frame_address(0));
	    void **bp = static_cast<void **>(* frame);
	    void *ip = frame[1];

	    for (; bp && ip && dladdr(ip, &info); ip = bp[1],
		     bp = static_cast<void **>(bp[0])) {
		std::ostringstream os;

		os << ip << " in " << demangle(info.dli_sname) << " ["
		   << info.dli_fname << ']';
		vec.push_back(os.str());

		if (info.dli_sname && ! strcmp(info.dli_sname, "main"))
		    break;
	    }

	    return vec;
	}

    }

}
