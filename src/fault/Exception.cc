#include <fault/Exception.hh>

#include <cxxabi.h>
#include <stdarg.h>
#include <sys/types.h>

#include <sstream>
#include <typeinfo>

class WrapperException : public fault::Exception {

private:
    const std::exception *exception;

protected:
    const std::string message() const
	throw()
    {
	return exception->what();
    }

protected:
    const Exception *clone() const
	throw()
    {
	return new WrapperException(*this);
    }

public:
    WrapperException(const std::exception *exception)
	throw()
	: fault::Exception(static_cast<const fault::Exception *>(NULL),
			   exception->what()),
	  exception(exception) { }

};

class Prefix {

private:
    size_t i;

public:
    Prefix(size_t i)
	throw()
	: i(i) { }

    void put(std::ostream& os) {
	if (i == 0)
	    return;

	os << '<';

	for (; i > 0; i--)
	    os << '-';

	os << ' ';
    }

};

std::ostream& operator<<(std::ostream& os, Prefix p) {
    p.put(os);

    return os;
}

Prefix prefix(int i)
    throw()
{
    return Prefix(i);
}

static const std::string buildMessage(const char *msg, va_list ap)
    throw()
{
    char buf[128];

    vsnprintf(buf, sizeof(buf), msg, ap);
    return buf;
}

fault::Exception::Exception(const Exception *reason, const char *msg, ...)
    throw()
    : reason(reason ? reason->clone() : NULL), trace(3)
{
    va_list ap;

    va_start(ap, msg);
    this->msg = buildMessage(msg, ap);
    va_end(ap);
}

fault::Exception::Exception(const std::exception *reason, const char *msg,
			    ...)
    throw()
    : reason(new WrapperException(reason)), trace(3)
{
    va_list ap;

    va_start(ap, msg);
    this->msg = buildMessage(msg, ap);
    va_end(ap);
}

fault::Exception::Exception(const char *msg, ...)
    throw()
    : reason(NULL), trace(3)
{
    va_list ap;

    va_start(ap, msg);
    this->msg = buildMessage(msg, ap);
    va_end(ap);
}

fault::Exception::~Exception()
    throw()
{
    if (reason != NULL)
	delete reason;
}

const std::string fault::Exception::message() const
    throw()
{
    std::ostringstream os;
    const Exception *e;
    size_t i;
    int status;

    for (i = 0, e = this; e != NULL; i++, e = e->reason) {
	os << prefix(i) << abi::__cxa_demangle(typeid(*e).name(), 0, 0, &status)
	   << ": " << e->msg << std::endl;
    }

    os << trace;

    return os.str();
}

const char *fault::Exception::what() const
    throw()
{
    return message().c_str();
}
