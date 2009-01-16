#ifndef _CHECKER_HH_
#define _CHECKER_HH_

#include "Result.hh"

#include <boost/filesystem.hpp>

class Checker {

public:
    virtual void check(boost::filesystem::path root, Result& result) const
	throw() = 0;

};

#endif /* _CHECKER_HH_ */
