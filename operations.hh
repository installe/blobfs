#ifndef _OPERATIONS_HH_
#define _OPERATIONS_HH_

#include "FileSystem.hh"

extern FileSystem* fs;

fuse_operations* operations()
    throw();

#endif /* _OPERATIONS_HH_ */
