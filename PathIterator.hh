#ifndef _PATHITERATOR_HH_
#define _PATHITERATOR_HH_

#include "PathIterator.hh"

#include <assert.h>

#include <algorithm>
#include <string>

using namespace std;

class PathIterator {

private:
    const string path;
    size_t beg;
    size_t end;

public:
    PathIterator(const string path)
	throw()
	: path(path)
    {
	assert(path[0] == '/');

	if (path != "/" && path != "") {
	    beg = 0;
	    end = path.find('/', beg + 1);
	} else {
	    beg = string::npos;
	}
    }

    bool hasNext()
	throw()
    {
	return beg != string::npos;
    }

    const string next()
	throw()
    {
	string result = path.substr(beg + 1, end - 1 - beg);

	beg = end;
	end = path.find('/', beg + 1);

	return result;
    }

};

#endif /* _PATHITERATOR_HH_ */
