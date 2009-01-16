#ifndef _DIRCHECKER_HH_
#define _DIRCHECKER_HH_

#include "NodeChecker.hh"

#include <boost/filesystem/path.hpp>

class DirChecker : public NodeChecker {

public:
    DirChecker(boost::filesystem::path root, boost::filesystem::path relPath)
	throw();

    void check(boost::filesystem::path root, Result& result) const
	throw();

};

#endif /* _DIRCHECKER_HH_ */
