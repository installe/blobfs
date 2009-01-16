#ifndef _USERINFO_HH_
#define _USERINFO_HH_

#include "InvalidArgumentError.hh"

#include <pwd.h>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

class UserInfo {

private:
    struct passwd pwent;

public:
    UserInfo(uid_t uid)
	throw(InvalidArgumentError);

    fs::path home() const
	throw();
	
};

#endif /* _USERINFO_HH_ */
