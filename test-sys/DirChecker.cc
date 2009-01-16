#include "DirChecker.hh"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

DirChecker::DirChecker(boost::filesystem::path root, boost::filesystem::path relPath)
    throw()
    : NodeChecker(relPath, 0755)
{
    boost::filesystem::create_directory(root / getRelPath());
}

void DirChecker::check(boost::filesystem::path root, Result& result) const
    throw()
{
    NodeChecker::check(root, result);

    boost::filesystem::path path = root / getRelPath();

    assertTrue(result, boost::filesystem::exists(path));
    assertTrue(result, boost::filesystem::is_directory(path));
}
