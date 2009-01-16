#ifndef _LOCKABLEMOCK_
#define _LOCKABLEMOCK_

class LockableMock {

private:
    bool locked;

public:
    LockableMock() : locked(false) { }

    void lock() {
	if (locked)
	    throw "Already locked";
	locked = true;
    }

    bool try_lock() {
	if (locked)
	    return false;
	locked = true;
	return true;
    }

    void unlock() {
	locked = false;
    }

    bool isLocked() {
	return locked;
    }

};

#endif /* _LOCKABLEMOCK_ */
