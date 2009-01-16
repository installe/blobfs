#include "ResultImpl.hh"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <sstream>

Failure::Failure(std::string file, int line, std::string msg)
    throw()
    : file(file), line(line), msg(msg) { }

std::string Failure::toString() const
    throw()
{
    return file + "(" + boost::lexical_cast<std::string>(line) + "): " + msg;
}

void ResultImpl::reportFailure(std::string file, int line, std::string msg)
    throw()
{
    failures.push_back(new Failure(file, line, msg));
}

class AppendFailure {

private:
    boost::shared_ptr<std::ostringstream> os;

public:
    AppendFailure()
	throw()
	: os(new std::ostringstream()) { }

    AppendFailure(AppendFailure& af)
	throw()
	: os(af.os) { }

    void operator()(const Failure *failure)
	throw()
    {
	*os << failure->toString() << std::endl;
    }

    operator std::string()
	throw()
    {
	return os->str();
    }

};

std::string ResultImpl::toString() const
    throw()
{
    AppendFailure appendFailure;

    for_each(failures.begin(), failures.end(), appendFailure);
    return appendFailure;
}

class DeleteFailure {

public:
    void operator()(Failure *failure) const
	throw()
    {
	delete failure;
    }
};

ResultImpl::~ResultImpl()
    throw()
{
    DeleteFailure deleteFailure;

    for_each(failures.begin(), failures.end(), deleteFailure);
}
