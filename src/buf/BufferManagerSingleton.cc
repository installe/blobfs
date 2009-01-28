#include <buf/BufferManager.hh>
#include <buf/BufferManagerSingleton.hh>

#include "impl/BufferManagerImpl.hh"

boost::mutex buf::BufferManagerSingleton::lock;

buf::BufferManager *buf::BufferManagerSingleton::instance = NULL;

buf::BufferManager& buf::BufferManagerSingleton::getInstance()
    throw()
{
    boost::lock_guard<boost::mutex> guard(lock);

    if (instance == NULL)
	instance = new buf::impl::BufferManagerImpl();

    return *instance;
}
