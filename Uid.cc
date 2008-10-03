#include "Uid.hh"

#include <cstring>

Uid::Uid()
    throw()
{
    memset(fpClipID, 0, sizeof(fpClipID));
}

Uid::Uid(const FPClipID _fpClipID)
    throw()
{
    memcpy(fpClipID, _fpClipID, sizeof(fpClipID));
}

Uid::Uid(const Uid& uid)
    throw()
{
    memcpy(fpClipID, uid.fpClipID, sizeof(fpClipID));
}

Uid& Uid::operator=(const Uid& uid)
    throw()
{
    memcpy(fpClipID, uid.fpClipID, sizeof(fpClipID));
    return *this;
}

Uid& Uid::operator=(const string& s)
    throw()
{
    memcpy(fpClipID, s.c_str(), sizeof(fpClipID));
    return *this;
}

const FPClipID& Uid::getClipID() const
    throw()
{
    return fpClipID;
}
