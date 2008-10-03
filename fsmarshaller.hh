#ifndef _FSMARSHALLER_HH_
#define _FSMARSHALLER_HH_

#include "filehandle.hh"
#include "IoError.hh"
#include "NodeHandleIterator.hh"
#include "Uid.hh"

#include <string>
#include <vector>

using namespace std;

class File;
class FileWriter;

class FsMarshaller {

public:

    virtual ~FsMarshaller()
	throw() { };

    // File related methods

    virtual FileHandle storeFile(const DirHandle dirHandle,
				 const FileWriter& writer)
	throw(IoError) = 0;

    virtual FileWriter& loadFile(const FileHandle fHandle) const
	throw(IoError) = 0;

    virtual void closeFileHandle(const FileHandle fHandle) const
	throw(IoError) = 0;

    virtual FileHandle copyFileHandle(const DirHandle dirHandle,
				      const FileHandle fHandle) const
	throw(IoError) = 0;

    virtual size_t getFileSize(const FileHandle fHandle) const
	throw(IoError) = 0;

    virtual void setFileStringAttr(const FileHandle fHandle, const string key,
				   const string value) const
	throw(IoError) = 0;

    virtual string getFileStringAttr(const FileHandle fHandle,
				     const string key) const
	throw(IoError) = 0;

    virtual void setFileLongAttr(const FileHandle fHandle, const string key,
				 long value) const
	throw(IoError) = 0;

    virtual long getFileLongAttr(const FileHandle fHandle, const string key)
	const
	throw(IoError) = 0;

    // RootDir related methods

    virtual const DirHandle openDirHandle(const Uid uid)
	throw(IoError) = 0;

    virtual const DirHandle newDirHandle()
	throw(IoError) = 0;

    virtual void closeDirHandle(const DirHandle dirHandle) const
	throw(IoError) = 0;

    virtual NodeHandleIterator getHandleIterator(const DirHandle dirHandle)
	const
	throw(IoError) = 0;

    // TODO Remove
//     virtual vector<Node*> loadRootDir(const DirHandle dirHandle) const
// 	throw(IoError) = 0;

//     virtual void addFileHandle(const DirHandle dirHandle,
// 			       const FileHandle fHandle) const
// 	throw(IoError) = 0;

    virtual const Uid storeRootDir(const DirHandle dirHandle)
	throw(IoError) = 0;

    // Dir related methods

};

#endif /* _FSMARSHALLER_HH_ */
