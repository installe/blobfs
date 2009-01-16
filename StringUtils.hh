#include "InvalidArgumentError.hh"

#include <string.h>

#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>

using namespace std;

string buildMessage(const string msg, va_list ap)
    throw();

template<class T> T parse(const string& s)
    throw(InvalidArgumentError)
{
    istringstream iss(s);
    T t;

    iss >> t;

    if (iss.fail() || ! iss.eof()) {
	throw InvalidArgumentError("<%s> is not a valid %s", s.c_str(),
				   typeid(T).name());
    }

    return t;
}
