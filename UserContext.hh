#ifndef _USERCONTEXT_HH_
#define _USERCONTEXT_HH_

#include <fuse.h>
#include <sys/stat.h>

class UserContext {

private:
    const struct fuse_context *context;

public:
    UserContext()
	throw();

    const uid_t getUid() const
	throw();

    const gid_t getGid() const
	throw();

};

#endif /* _USERCONTEXT_HH_ */
