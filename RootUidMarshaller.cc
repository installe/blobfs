#include <fstream>

#include "RootUidMarshaller.hh"

using namespace std;

RootUidMarshaller::RootUidMarshaller(const string fName)
    throw()
    : fName(fName) { }

RootUidMarshaller::RootUidMarshaller(RootUidMarshaller& marshaller)
    throw()
    : fName(marshaller.fName) { }

RootUidMarshaller& RootUidMarshaller::operator=(const RootUidMarshaller& marshaller)
    throw()
{
    fName = marshaller.fName;
    return *this;
}

RootUidMarshaller::~RootUidMarshaller()
    throw() { }

bool RootUidMarshaller::loadRootUid(Uid& outUid) const
    throw(IoError)
{
    ifstream ifs(fName.c_str());
    string s;

    if (! ifs)
	return false;

    ifs >> s;

    if (s.length() == 0)
	return false;

    outUid = s;
    return true;
}

void RootUidMarshaller::storeRootUid(const Uid uid) const
    throw(IoError)
{
    ofstream ofs(fName.c_str());

    ofs << uid.getClipID() << endl;
}
