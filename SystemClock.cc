#include "SystemClock.hh"

#include <time.h>

time_t SystemClock::time() const
    throw()
{
    return ::time(NULL);
}
