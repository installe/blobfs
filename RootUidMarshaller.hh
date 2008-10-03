#ifndef _ROOTUIDMARSHALLER_HH_
#define _ROOTUIDMARSHALLER_HH_

#include "IoError.hh"
#include "Uid.hh"

class RootUidMarshaller {

private:
    string fName;

public:
    RootUidMarshaller(const string fName)
	throw();

    RootUidMarshaller& operator=(const RootUidMarshaller& marshaller)
	throw();

    RootUidMarshaller(RootUidMarshaller& marshaller)
	throw();

    ~RootUidMarshaller()
	throw();

    bool loadRootUid(Uid& outUid) const
	throw(IoError);

    void storeRootUid(const Uid uid) const
	throw(IoError);

};

#endif /* _ROOTUIDMARSHALLER_HH_ */
