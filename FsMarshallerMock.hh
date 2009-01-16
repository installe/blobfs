#ifndef _FSMARSHALLERMOCK_HH_
#define _FSMARSHALLERMOCK_HH_

#include "filehandle.hh"
#include "fsmarshaller.hh"
#include "IoError.hh"

#include <string>

using std::string;

class FileWriter;

class HandleIteratorMock : public HandleIterator {

    bool hasNext() throw(IoError) { return false; }

    FileHandle next() throw(IoError) { throw IoError(42, "test"); }

};

class FsMarshallerMock : public FsMarshaller {
    FileHandle storeFile(const DirHandle dirHandle, const FileWriter& writer)
	throw(IoError) { return FileHandle(0); }

    FileWriter& loadFile(const FileHandle fHandle) const
	throw(IoError) { return *new FileWriter(); }
	
    void closeFileHandle(const FileHandle fHandle) const
	throw(IoError) { }

    FileHandle copyFileHandle(const DirHandle dirHandle, const FileHandle fHandle)
	const
	throw(IoError) { return FileHandle(0); }

    size_t getFileSize(const FileHandle fHandle) const
	throw(IoError) { return -1; }

    void setStringAttr(const FileHandle fHandle, const string key,
		       const string value) const
	throw(IoError) { }

    string getStringAttr(const FileHandle fHandle, const string key) const
	throw(IoError) { return key; }

    void setLongAttr(const FileHandle fHandle, const string key, long value)
	const
	throw(IoError) { }

    long getLongAttr(const FileHandle fHandle, const string key) const
	throw(IoError) { return -1; }

    const DirHandle openDirHandle(const Uid uid)
	throw(IoError) { return DirHandle(0); }

    const DirHandle newDirHandle()
	throw(IoError) { return DirHandle(0); }

    void closeDirHandle(const DirHandle dirHandle) const
	throw(IoError) { }

    NodeHandleIterator getHandleIterator(const DirHandle dirHandle) const
	throw(IoError) { return NodeHandleIterator(*new HandleIteratorMock()); }

    const Uid storeRootDir(const DirHandle dirHandle)
	throw(IoError) { return Uid("rootDir"); }

    const Uid storeDir(const DirHandle dirHandle)
	throw(IoError) { return Uid("dir"); }

    FileHandle toFileHandle(const DirHandle dirHandle)
	throw(IoError) { return FileHandle(0); }

    FileHandle addSubdir(const DirHandle dirHandle, const Uid uid)
	throw(IoError) { return FileHandle(0); }

};

#endif /* _FSMARSHALLERMOCK_HH_ */
