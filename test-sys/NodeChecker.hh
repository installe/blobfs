#ifndef _NODECHECKER_HH_
#define _NODECHECKER_HH_

#include "Checker.hh"
#include "Result.hh"

#include <boost/filesystem.hpp>
#include <exception>

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

    void setCTime(boost::filesystem::path root, time_t time)
	throw(std::exception);

    void setMTime(boost::filesystem::path root, time_t time)
	throw(std::exception);

    void setATime(boost::filesystem::path root, time_t time)
	throw(std::exception);

    void setUid(boost::filesystem::path root, uid_t uid)
	throw(std::exception);

    void setGid(boost::filesystem::path root, gid_t gid)
	throw(std::exception);

    void setMode(boost::filesystem::path root, mode_t mode)
	throw(std::exception);

};

#endif /* _NODECHECKER_HH_ */
