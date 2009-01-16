#include "Parent.hh"

LockGuard<> Parent::guard()
    throw()
{
    LockGuard<> l(m);

    return l;
}
