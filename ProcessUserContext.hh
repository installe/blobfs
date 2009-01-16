#ifndef _PROCESSUSERCONTEXT_HH_
#define _PROCESSUSERCONTEXT_HH_

#include <sys/stat.h>

class ProcessUserContext {

private:
    const uid_t uid;
    const gid_t gid;
    const mode_t mode;

    const mode_t calcMode() const
	throw();

public:
    ProcessUserContext()
	throw();

    const uid_t getUid() const
	throw();

    const gid_t getGid() const
	throw();

    const mode_t getMode() const
	throw();

};

#endif /* _PROCESSUSERCONTEXT_HH_ */
