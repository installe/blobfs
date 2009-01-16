#ifndef _BLOBFSGUARD_HH_
#define _BLOBFSGUARD_HH_

#include <boost/filesystem.hpp>

class BlobfsGuard {

private:
    boost::filesystem::path root;

    void mount()
	throw(std::exception);

    void unmount()
	throw(std::exception);

public:
    BlobfsGuard()
	throw();

    ~BlobfsGuard()
	throw();

    boost::filesystem::path getRoot() const
	throw();

};

#endif /* _BLOBFSGUARD_HH_ */
