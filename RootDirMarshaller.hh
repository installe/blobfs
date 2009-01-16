#ifndef _ROOTDIRMARSHALLER_HH_
#define _ROOTDIRMARSHALLER_HH_

#include "file.hh"
#include "fsmarshaller.hh"
#include "IoError.hh"
#include "Uid.hh"

class RootDirMarshaller {

public:
    virtual void marshal(const RootDir& rootDir, const Uid& uid) const
	throw(IoError) = 0;

    virtual RootDir& unmarshal(FsMarshaller& marshaller) const
	throw(IoError) = 0;

    virtual ~RootDirMarshaller() { }

};

#endif /* _ROOTDIRMARSHALLER_HH_ */
