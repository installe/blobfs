#include "InvalidArgumentError.hh"
#include "StringUtils.hh"

#include <stdarg.h>

InvalidArgumentError::InvalidArgumentError(const string msg, ...)
    throw()
{
    va_list ap;

    va_start(ap, msg);
    this->msg = buildMessage(msg, ap);
    va_end(ap);
}

InvalidArgumentError::~InvalidArgumentError()
    throw() { }

const char *InvalidArgumentError::what() const
    throw()
{
    return msg.c_str();
}
