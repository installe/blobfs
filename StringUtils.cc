#include "StringUtils.hh"

#include <stdlib.h>

// TODO This is a glaring memory leak! Fix!
string buildMessage(const string msg, va_list ap)
    throw()
{
    size_t maxsize = 128;
    char *buf = static_cast<char *>(malloc(maxsize));
    size_t size = vsnprintf(buf, maxsize, msg.c_str(), ap);

    if (size >= maxsize) {
	buf = static_cast<char *>(realloc(buf, size));
	vsnprintf(buf, maxsize, msg.c_str(), ap);
    }

    string s = string(buf);

    return s;
}
