#ifndef _BLOBFSMARSHALLER_HH_
#define _BLOBFSMARSHALLER_HH_

#include "fsmarshaller.hh"
#include "NodeHandleIterator.hh"
#include "RootUidMarshaller.hh"

#include <FPAPI.h>
#include <log4cpp/Category.hh>

#include <string>

using namespace std;

class BlobFsMarshaller : public FsMarshaller {

private:
    static log4cpp::Category& cat;

    string address;
    bool connected;
    FPPoolRef pool;
    RootUidMarshaller rootUidMarshaller;

    void connectPool()
	throw(IoError);
    
    void disconnectPool()
	throw(IoError);

    void checkForErrors(string action, ...) const
	throw(IoError);

public:
    BlobFsMarshaller(RootUidMarshaller rootUidMarshaller, string address)
	throw();

    ~BlobFsMarshaller()
	throw();

    // File related methods

    FileHandle storeFile(const DirHandle dirHandle, const FileWriter& writer)
	throw(IoError);

    FileWriter& loadFile(const FileHandle fHandle) const
	throw(IoError);

    void closeFileHandle(const FileHandle fHandle) const
	throw(IoError);

    FileHandle copyFileHandle(const DirHandle dstHandle,
			      const FileHandle fHandle) const
	throw(IoError);

    size_t getFileSize(const FileHandle fHandle) const
	throw(IoError);

    void setFileStringAttr(const FileHandle fHandle, const string key,
			   const string value) const
	throw(IoError);

    string getFileStringAttr(const FileHandle fHandle, const string key) const
	throw(IoError);

    void setFileLongAttr(const FileHandle fHandle, const string key,
			 long value) const
	throw(IoError);

    long getFileLongAttr(const FileHandle fHandle, const string key) const
	throw(IoError);

    // Rootdir related methods

    const DirHandle openDirHandle(const Uid uid)
	throw(IoError);

    const DirHandle newDirHandle()
	throw(IoError);

    void closeDirHandle(const DirHandle dirHandle) const
	throw(IoError);

    NodeHandleIterator getHandleIterator(const DirHandle dirHandle) const
	throw(IoError);

    const Uid storeRootDir(const DirHandle dirHandle)
	throw(IoError);

};

#endif /* _BLOBFSMARSHALLER_HH_ */
