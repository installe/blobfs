#include <fault/Stacktrace.hh>

#include <cxxabi.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

static const std::string demangle(const char *name)
    throw()
{
    int status = 0;
    char *d = 0;
    std::string ret = name;

    try {
	if ((d = abi::__cxa_demangle(name, 0, 0, &status)))
	    ret = d;
    } catch (...) {
	
    }

    free(d);
    return ret;
}

static const std::vector<std::string> trace()
    throw()
{
    std::vector<std::string> vec;
    Dl_info info;
    void **frame = static_cast<void **>(__builtin_frame_address(0));
    void **bp = static_cast<void **>(*frame);
    void *ip = frame[1];

    while (bp && ip && dladdr(ip, &info)) {
	std::ostringstream os;

	os << ip << " in " << demangle(info.dli_sname) << " [" << info.dli_fname
	   << ']';

	vec.push_back(os.str());

	if (info.dli_sname && ! strcmp(info.dli_sname, "main"))
	    break;

	ip = bp[1];
	bp = static_cast<void **>(bp[0]);
    }

    return vec;
}

fault::Stacktrace::Stacktrace(size_t skip)
    throw()
    : stacktrace(trace()), skip(skip) { }

namespace fault {

    template<class T> class OutputDumper {

    private:
	const size_t skip;
	std::ostream& os;
	size_t frameNo;

    public:
	OutputDumper(std::ostream& os, size_t skip)
	    throw()
	    : skip(skip), os(os), frameNo(0) { }

	void operator()(const T& t)
	    throw()
	{
	    if (frameNo >= skip)
		os << "#" << frameNo << " " << t << std::endl;
	    ++frameNo;
	}

    };

}

void fault::Stacktrace::put(std::ostream& os) const
    throw()
{
    typedef std::ostream& (*output_fcn)(std::ostream& os, const std::string& s);
    std::for_each(stacktrace.begin(), stacktrace.end(),
		  fault::OutputDumper<std::string>(os, skip));
}

std::ostream& fault::operator<<(std::ostream& os, const fault::Stacktrace& trace)
    throw()
{
    trace.put(os);
    return os;
}
