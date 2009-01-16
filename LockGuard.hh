#ifndef _LOCKGUARD_HH_
#define _LOCKGUARD_HH_

#include <boost/shared_ptr.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>

using boost::lock_guard;
using boost::shared_ptr;
using boost::mutex;

template <class lockable = mutex> class LockGuard {

private:
    shared_ptr< lock_guard <lockable> > locker;

public:
    LockGuard(lockable& lock)
	throw() : locker(new lock_guard<lockable>(lock)) { }

    // TODO Remove
//     LockGuard(LockGuard& guard)
// 	throw() : locker(guard.locker) { }

    LockGuard(const LockGuard& guard)
	throw() : locker(guard.locker) { }

    ~LockGuard()
	throw() { }

    // TODO Remove
//     LockGuard& operator=(LockGuard guard)
// 	throw()
//     {
// 	locker = guard.locker;

// 	return *this;
//     }
    
};

#endif 
