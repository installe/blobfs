#include "BlobfsGuard.hh"

BlobfsGuard::BlobfsGuard()
    throw()
    : root("/tmp/fuse")
{
    mount();
}

BlobfsGuard::~BlobfsGuard()
    throw()
{
    unmount();
}

boost::filesystem::path BlobfsGuard::getRoot() const
    throw()
{
    return root;
}

void BlobfsGuard::mount()
    throw(std::exception)
{
    std::string cmd = "../test_fuse 128.221.200.56?/home/arnaud/Desktop/Downloads/emea1_profile1_rdqeDcw.pea "
	+ root.string() + " -s -o allow_other";

    boost::filesystem::create_directory(root);
    if (system(cmd.c_str()))
	throw std::exception();
}

void BlobfsGuard::unmount()
    throw(std::exception)
{
    std::string cmd = "sudo fusermount -u " + root.string();

    if (system(cmd.c_str()))
	throw std::exception();
}
