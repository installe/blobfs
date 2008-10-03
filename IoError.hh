#ifndef _IOERROR_HH_
#define _IOERROR_HH_

#include <exception>
#include <string>

using namespace std;

class IoError : public exception {

private:
    int error;
    string msg;
    const exception* reason;

public:
    IoError(int error, const string msg, ...) throw();

    IoError(int error, const exception *reason, const string msg, ...) throw();

    const char *what() const throw();

    int errcode() const throw();

    ~IoError() throw();

};

#endif /* _IOERROR_HH_ */
