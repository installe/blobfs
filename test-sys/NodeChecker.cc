#include "NodeChecker.hh"

#include <boost/lexical_cast.hpp>
#include <sys/stat.h>
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
    assertTrue(result, abs(expected - actual) <= 5, attrName
	       + ": Expected: " + boost::lexical_cast<std::string>(expected)
	       + ", Actual: " + boost::lexical_cast<std::string>(actual));
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

boost::filesystem::path NodeChecker::getRelPath() const
    throw()
{
    return relPath;
}
