#ifndef _SYSTEMCLOCK_HH_
#define _SYSTEMCLOCK_HH_

#include "Clock.hh"

class SystemClock : public Clock {

public:
    time_t time() const
	throw();

};

#endif /* _SYSTEMCLOCK_HH_ */
