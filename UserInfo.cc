#include "UserInfo.hh"

#include <pwd.h>

class PwentIterator {

public:
    ~PwentIterator()
	throw()
    {
	endpwent();
    }

    passwd *next() const
	throw()
    {
	return getpwent();
    }

};

UserInfo::UserInfo(uid_t uid)
    throw(InvalidArgumentError)
{
    PwentIterator iter;

    while (struct passwd *it = iter.next()) {
	if (it->pw_uid == uid) {
	    pwent = *it;
	    return;
	}
    }

    throw InvalidArgumentError("User with id %i doesn't exist.", uid);
}

fs::path UserInfo::home() const
    throw()
{
    return fs::path(pwent.pw_dir);
}
