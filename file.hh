#ifndef _FILE_HH_
#define _FILE_HH_

#include "Child.hh"
#include "filedata.hh"
#include "filehandle.hh"
#include "fsmarshaller.hh"
#include "IoError.hh"
#include "NodeAttrImpl.hh"
#include "Parent.hh"

#include <FPAPI.h>
#include <log4cpp/Category.hh>
#include <string>

using namespace std;

class ChildIterator;

class RootDirMarshaller;
class FileState;
class Uid;

class File;

class DirMarshallerException;

// TODO Split API/implementation

class FileState {

public:
    virtual ssize_t getSize() const
	throw() = 0;

    virtual size_t read(void *buf, ssize_t offset, size_t size) const
	throw(IoError) = 0;

    virtual size_t write(const void *buf, ssize_t offset, size_t size)
	throw(IoError) = 0;

    virtual FileHandle addToDirHandle(DirHandle dirHandle)
	throw(IoError) = 0;

    virtual void release()
	throw(IoError) = 0;

    virtual ~FileState()
	throw() { };

};

//TODO Move implementation of File/DirHandle to .cc code.
class File : public Child {

private:
    FsMarshaller& marshaller;
    FileState* state;

    static log4cpp::Category& cat;

    File(File& file); // don't use

    File& operator=(const File& file); // don't use

public:
    File(FsMarshaller& marshaller, Parent& parent, const string name) throw();

    File(FsMarshaller& marshaller, Parent& parent, const string name,
	 const FileHandle fHandle) throw(IoError);

    FsMarshaller& getMarshaller() const throw();

    ssize_t getSize() const throw();

    const FileState& getState() const throw();

    void setState(FileState& state) throw();

    size_t read(void *buf, ssize_t offset, size_t size) throw(IoError);

    size_t write(const void *buf, ssize_t offset, size_t size) throw(IoError);

    FileHandle addToDirHandle(DirHandle dirHandle) throw(IoError);

    void release() throw(IoError);

    virtual ~File() throw();

};

class RootDirState {

public:
    virtual ChildIterator childIterator()
	throw(IoError) = 0;

    virtual void addChild(Child& child)
	throw(IoError) = 0;

    virtual void removeChild(Child& child)
	throw(IoError) = 0;

    virtual void setDirty()
	throw(IoError) = 0;

    virtual void release()
	throw(IoError) = 0;

    virtual ~RootDirState()
	throw() { };

};

class RootDir : public Parent {

private:
    FsMarshaller &marshaller;
    const RootDirMarshaller &rootDirMarshaller;
    RootDirState *state;
    NodeAttrImpl attr;

    static log4cpp::Category& cat;

    RootDir(RootDir& rootDir)
	throw(); // don't use

    RootDir& operator=(const RootDir& rootDir)
	throw(); // don't use

public:
    // Ctors

    RootDir(FsMarshaller& marshaller, const RootDirMarshaller& rootDirMarshaller)
	throw();

    RootDir(FsMarshaller& marshaller, const RootDirMarshaller& rootDirMarshaller, Uid uid)
	throw(IoError);

    ~RootDir()
	throw();

    // Introspection

    FsMarshaller& getMarshaller() const
	throw();

    const RootDirMarshaller& getRootDirMarshaller() const
	throw();

    NodeAttr& getAttr()
	throw();

    const NodeAttr& getAttr() const
	throw();

    const Node& getOwner() const
	throw();

    const RootDirState& getState() const
	throw();

    void setState(RootDirState& state)
	throw();

    // Operations

    ChildIterator childIterator()
	throw(IoError);

    void addChild(Child& child)
	throw(IoError);

    void removeChild(Child& child)
	throw(IoError);

    void setDirty()
	throw(IoError);

    void release()
	throw(IoError);

};

// class Dir {

// private:
//     Uid uid;

//     DirHandle dirHandle;
//     vector<File*> files;

// public:
//     Dir(Dir* dir, string name, const Uid uid);

//     const DirHandle getDirHandle() const;

//     void setDirHandle(DirHandle dirHandle);

//     const Uid getUid() const;

//     void setUid(Uid uid);

//     void addChild(File *file);

//     vector<File*>* getFiles();
// };

class DirMarshallerException {

    string msg;

    static log4cpp::Category& cat;

public:
    DirMarshallerException(string msg, ...);

    string getMsg() const;

};

#endif /* _FILE_HH_ */
