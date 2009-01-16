#ifndef _LOCKGUARDSTACK_HH_
#define _LOCKGUARDSTACK_HH_

#include "LockGuard.hh"

#include <stack>

class mutex;

template <class lockable = mutex> class LockGuardStack {

private:
    std::stack<LockGuard> stack;

    LockGuardStack(LockGuardStack& stack)
	throw() { }

    LockGuard& operator=(LockGuardStack& stack)
	throw() { }

public:
    LockGuardStack()
	throw() { }

    ~LockGuardStack()
	thow()
    {
	while (! stack.empty())
	    stack.pop();
    }

    void push(const LockGuard& guard)
	throw()
    {
	stack.push(guard);
    }

    void pop()
	throw()
    {
	stack.pop();
    }

};

#endif /* _LOCKGUARDSTACK_HH_ */
