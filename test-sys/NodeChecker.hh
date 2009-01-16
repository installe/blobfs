#ifndef _NODECHECKER_HH_
#define _NODECHECKER_HH_

#include "Checker.hh"
#include "Result.hh"

#include <boost/filesystem.hpp>

class NodeChecker : public Checker {

private:
    boost::filesystem::path relPath;

    uid_t uid;
    gid_t gid;

    mode_t perms;

    time_t cTime;
    time_t mTime;
    time_t aTime;

protected:
    boost::filesystem::path getRelPath() const
	throw();

public:
    NodeChecker(boost::filesystem::path relPath, mode_t mode)
	throw();

    void check(boost::filesystem::path root, Result& result) const
	throw();

};

#endif /* _NODECHECKER_HH_ */
