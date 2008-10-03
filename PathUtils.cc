#include "PathIterator.hh"
#include "PathUtils.hh"

#include <assert.h>

using namespace std;

const string PathUtils::dirName(const string path) const
    throw()
{
    assert(path[0] == '/');

    size_t idx = path.rfind('/');

    return path.substr(0, idx > 0 ? idx : 1);
}

const string PathUtils::baseName(const string path) const
    throw()
{
    assert(path[0] == '/');

    size_t idx = path.rfind('/');

    return path.substr(idx + 1);
}

PathIterator PathUtils::pathIterator(const string path) const
    throw()
{
    assert(path[0] == '/');

    return PathIterator(path);
}
