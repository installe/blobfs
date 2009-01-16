#ifndef _CLOCK_HH_
#define _CLOCK_HH_

#include <sys/types.h>

class Clock {

public:
    virtual time_t time() const
	throw() = 0;

};

#endif /* _CLOCK_HH_ */
