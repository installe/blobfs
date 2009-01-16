#ifndef _LINEPARSER_HH_
#define _LINEPARSER_HH_

#include <stdarg.h>
#include <stdlib.h>

#include <exception>
#include <iostream>
#include <string>

using namespace std;

// TODO Move to util class?
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

// TODO Move to separate files
class ParseError : public exception {

private:
    string msg;

public:
    ParseError(const string msg, ...)
	throw()
    {
	va_list ap;

	va_start(ap, msg);
	this->msg = buildMessage(msg, ap);
	va_end(ap);
    }

    ~ParseError()
	throw() { }
       
    const char *what() const
	throw() { return msg.c_str(); }

};

class LineParser {

private:
    const string line;

    enum { 
	SKIP_INITIAL_SPACE, PARSE_KEY, SKIP_SPACE_AFTER_KEY, SKIP_EQUALS_SIGN,
	SKIP_SPACE_AFTER_EQUALS_SIGN, PARSE_VALUE, SKIP_SPACE_AFTER_VALUE
    } state;
    size_t idx;

    string key;
    string val;

    bool isvalidchar(int c)
	throw()
    {
	return c == '.' || isalnum(c);
    }

    void process(size_t i, const char c)
	throw(ParseError)
    {
	switch (state) {

	    case SKIP_INITIAL_SPACE:
		if (isspace(c)) {
		    // NOP
		} else if (isvalidchar(c)) {
		    state = PARSE_KEY;
		    idx = i;
		} else {
		    throw ParseError("Invalid character '%c': pos %zu line '%s'", c, i, line.c_str());
		}
		break;

	    case PARSE_KEY:
		if (isvalidchar(c)) {
		    // NOP
		} else if (isspace(c)) {
		    state = SKIP_SPACE_AFTER_KEY;
		    key = line.substr(idx, i - idx);
		} else if (c == '=') {
		    state = SKIP_EQUALS_SIGN;
		    key = line.substr(idx, i - idx);
		} else {
		    throw ParseError("Invalid character '%c': pos %zu line '%s'", c, i, line.c_str());
		}
		break;

	    case SKIP_SPACE_AFTER_KEY:
		if (isspace(c)) {
		    // NOP
		} else if (c == '=') {
		    state = SKIP_EQUALS_SIGN;
		} else {
		    throw ParseError("Invalid character '%c': pos %zu line '%s'", c, i, line.c_str());
		}
		break;

	    case SKIP_EQUALS_SIGN:
		if (isspace(c)) {
		    state = SKIP_SPACE_AFTER_EQUALS_SIGN;
		} else if (isvalidchar(c)) {
		    state = PARSE_VALUE;
		    idx = i;
		} else {
		    throw ParseError("Invalid character '%c': pos %zu line '%s'", c, i, line.c_str());
		}
		break;

	    case SKIP_SPACE_AFTER_EQUALS_SIGN:
		if (isspace(c)) {
		    // NOP
		} else if (isvalidchar(c)) {
		    state = PARSE_VALUE;
		    idx = i;
		} else {
		    throw ParseError("Invalid character '%c': pos %zu line '%s'", c, i, line.c_str());
		}
		break;

	    case PARSE_VALUE:
		if (isvalidchar(c)) {
		    // NOP
		} else if (isspace(c)) {
		    state = SKIP_SPACE_AFTER_VALUE;
		    val = line.substr(idx, i - idx);
		} else {
		    throw ParseError("Invalid character '%c': pos %zu line '%s'", c, i, line.c_str());		    
		}
		break;

	    case SKIP_SPACE_AFTER_VALUE:

		if (isspace(c)) {
		    // NOP
		} else {
		    throw ParseError("Invalid character '%c': pos %zu line '%s'", c, i, line.c_str());
		}
		break;

	    default:
		throw ParseError("Invalid state");

	}
    }

public:
    LineParser(string line)
	throw(ParseError)
	: line(line), state(SKIP_INITIAL_SPACE), idx(0)
    {
	size_t i;

	for (i = 0; i < line.length(); i++)
	    process(i, line[i]);
	process(i, ' ');
    }

    const string getKey() { return key; }

    const string getVal() { return val; }

};

#endif /* _LINEPARSER_HH_ */
