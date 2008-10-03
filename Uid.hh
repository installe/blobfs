#ifndef _UID_HH_
#define _UID_HH_

#include <FPAPI.h>

#include <string>

using namespace std;

class Uid {

private:
    FPClipID fpClipID;

public:
    Uid() throw();

    Uid(const FPClipID fpClipID) throw();

    Uid(const Uid& uid) throw();

    Uid& operator=(const Uid& uid) throw();

    Uid& operator=(const string& s) throw();

    const FPClipID& getClipID() const throw();

};

#endif /* _UID_HH_ */
