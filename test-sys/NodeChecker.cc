#include "NodeChecker.hh"

#include <boost/lexical_cast.hpp>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

template <class T> void checkEquals(Result& result,
				    boost::filesystem::path& path, 
				    T expected, T actual,
				    const std::string attrName)
    throw()
{
    assertEquals<T>(result, expected, actual,
		    attrName + " of file <" + path.string() + ">");
}

void checkTimeEquals(Result& result,
		     boost::filesystem::path& path,
		     time_t expected, time_t actual,
		     const std::string attrName)
    throw()
{
    assertTrue(result, abs(expected - actual) <= 5, attrName + " for <"
	       + path.string().c_str() + ">: Expected: "
	       + boost::lexical_cast<std::string>(expected) + ", Actual: "
	       + boost::lexical_cast<std::string>(actual) + ", Difference: "
	       + boost::lexical_cast<std::string>(actual - expected));
}

NodeChecker::NodeChecker(boost::filesystem::path relPath, mode_t mode)
    throw()
    : relPath(relPath), perms(mode)
{
    time_t now = time(NULL);

    uid = getuid();
    gid = getgid();
    cTime = mTime = aTime = now;
}

void NodeChecker::check(boost::filesystem::path root, Result& result) const
    throw()
{
    struct stat st;
    boost::filesystem::path path = root / relPath;

    assertTrue(result, boost::filesystem::exists(path));

    int res = stat(path.string().c_str(), &st);

    assertEquals(result, 0, res, "Stat failed");

    checkEquals<uid_t>(result, path, uid, st.st_uid, "uid");
    checkEquals<gid_t>(result, path, gid, st.st_gid, "gid");
    checkEquals<mode_t>(result, path, perms, st.st_mode & ~S_IFMT,
			"permissions");
    checkTimeEquals(result, path, cTime, st.st_ctime, "ctime");
    checkTimeEquals(result, path, mTime, st.st_mtime, "mtime");
    checkTimeEquals(result, path, aTime, st.st_atime, "atime");
}

void NodeChecker::setCTime(boost::filesystem::path root, time_t time)
    throw(std::exception)
{

}

void NodeChecker::setMTime(boost::filesystem::path root, time_t time)
    throw(std::exception)
{
    boost::filesystem::last_write_time(root, time);
    mTime = time;
}

void NodeChecker::setATime(boost::filesystem::path root, time_t time)
    throw(std::exception)
{
    boost::filesystem::path path = root / getRelPath();
    const char* pathName = path.string().c_str();
    struct stat st;
    struct timeval times[2];

    times[0].tv_usec = times[1].tv_usec = 0;

    if (stat(pathName, &st))
	throw std::exception();

    times[0].tv_sec = time;
    times[1].tv_sec = st.st_mtime;

    if (utimes(pathName, times))
	throw std::exception();

    aTime = time;
}

void NodeChecker::setUid(boost::filesystem::path root, uid_t uid)
    throw(std::exception)
{
    boost::filesystem::path path = root / getRelPath();

    if (chown(path.string().c_str(), uid, -1))
	this->uid = uid;
}

void NodeChecker::setGid(boost::filesystem::path root, gid_t gid)
    throw(std::exception)
{
    boost::filesystem::path path = root / getRelPath();

    if (chown(path.string().c_str(), -1, gid))
	this->gid = gid;
}

void NodeChecker::setMode(boost::filesystem::path root, mode_t mode)
    throw(std::exception)
{
    boost::filesystem::path path = root / getRelPath();

    if (chmod(path.string().c_str(), mode))
	this->perms = mode;
}

boost::filesystem::path NodeChecker::getRelPath() const
    throw()
{
    return relPath;
}
