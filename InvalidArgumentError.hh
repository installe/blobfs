#ifndef _INVALIDARGUMENTERROR_HH_
#define _INVALIDARGUMENTERROR_HH_

#include <string>

using namespace std;

class InvalidArgumentError : public exception {

private:
    string msg;

public:
    InvalidArgumentError(const string msg, ...) throw();

    const char *what() const throw();

    ~InvalidArgumentError() throw();

};

#endif /* _INVALIDARGUMENTERROR_HH_ */
