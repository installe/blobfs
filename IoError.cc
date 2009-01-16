#include "IoError.hh"
#include "StringUtils.hh"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <string>

using namespace std;

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
