#ifndef _ROOTDIRMARSHALLERIMPL_HH_
#define _ROOTDIRMARSHALLERIMPL_HH_

#include "fsmarshaller.hh"
#include "RootDirMarshaller.hh"

#include <boost/filesystem.hpp>

#include <string>

using namespace std;

class RootDirMarshallerImpl : public RootDirMarshaller {

private:
    static log4cpp::Category& cat;

    const boost::filesystem::path path;

public:
    RootDirMarshallerImpl(const boost::filesystem::path path)
	throw();

    void marshal(const RootDir& rootDir, const Uid& uid) const
	throw(IoError);

    RootDir& unmarshal(FsMarshaller& marshaller) const
	throw(IoError);

};

#endif /* _ROOTDIRMARSHALLERIMPL_HH_ */
