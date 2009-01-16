#ifndef _BLOBFSMARSHALLER_HH_
#define _BLOBFSMARSHALLER_HH_

#include "fsmarshaller.hh"
#include "NodeHandleIterator.hh"

#include <FPAPI.h>
#include <log4cpp/Category.hh>

#include <string>

using namespace std;

class BlobFsMarshaller : public FsMarshaller {

private:
    static log4cpp::Category& cat;

    string address;
    FPPoolRef pool;

    void connectPool()
	throw(IoError);
    
    void disconnectPool()
	throw(IoError);

    void checkForErrors(string action, ...) const
	throw(IoError);

public:
    BlobFsMarshaller(string address)
	throw(IoError);

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

    void setStringAttr(const FileHandle fHandle, const string key,
		       const string value) const
	throw(IoError);

    string getStringAttr(const FileHandle fHandle, const string key) const
	throw(IoError);

    void setLongAttr(const FileHandle fHandle, const string key,
		     long value) const
	throw(IoError);

    long getLongAttr(const FileHandle fHandle, const string key) const
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

    // Dir related methods

    const Uid storeDir(const DirHandle dirHandle)
	throw(IoError);

    // Node conversion methods

    FileHandle toFileHandle(const DirHandle dirHandle)
	throw(IoError);

    FileHandle addSubdir(const DirHandle dirHandle, const Uid uid)
	throw(IoError);

};

#endif /* _BLOBFSMARSHALLER_HH_ */
