#ifndef _PATHUTILS_HH_
#define _PATHUTILS_HH_

#include "PathIterator.hh"

#include <string>

using namespace std;

class PathUtils {

public:
    const string dirName(const string path) const
	throw();

    const string baseName(const string path) const
	throw();

    PathIterator pathIterator(const string path) const
	throw();
};

#endif /* _PATHUTILS_HH_ */
