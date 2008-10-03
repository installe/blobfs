#include "IoError.hh"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

using namespace std;

static string buildMessage(const string msg, va_list ap)
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

IoError::IoError(int error, const string msg, ...)
    throw()
    : error(error), reason(NULL)
{
    va_list ap;

    va_start(ap, msg);
    this->msg = buildMessage(msg, ap);
    va_end(ap);
}

IoError::IoError(int error, const exception *reason, const string msg, ...)
    throw()
    : error(error), msg(msg), reason(reason)
{
    va_list ap;

    va_start(ap, msg);
    this->msg = buildMessage(msg, ap);
    va_end(ap);
} 

const char *IoError::what() const
    throw()
{
    string s = msg + ": " + strerror(error);

    if (reason != NULL)
	s += reason->what();

    return s.c_str();
}

int IoError::errcode() const
    throw()
{
    return error;
}

IoError::~IoError() throw() { }
