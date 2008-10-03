#include "BlobFsHandleIterator.hh"
#include "BlobFsMarshaller.hh"
#include "file.hh"
#include "filedata.hh"
#include "logutils.hh"
#include "RootUidMarshaller.hh"

#include <alloca.h>
#include <assert.h>
#include <errno.h>

log4cpp::Category& BlobFsMarshaller::cat =
    log4cpp::Category::getInstance("BlobFsMarshaller");

BlobFsMarshaller::BlobFsMarshaller(RootUidMarshaller marshaller, string address)
    throw()
    : address(address), connected(false), rootUidMarshaller(marshaller) { }

BlobFsMarshaller::~BlobFsMarshaller()
    throw()
{
    disconnectPool();
}

void BlobFsMarshaller::connectPool()
    throw(IoError)
{
    if (connected)
	return;

    cat.debug("Connecting to pool at address <%s>", address.c_str());
    pool = FPPool_Open(address.c_str());

    checkForErrors("connect to %s", address.c_str());
    assert(pool != 0);

    cat.debug("Connected to pool at address <%s>", address.c_str());
    connected = true;
}

void BlobFsMarshaller::disconnectPool()
    throw(IoError)
{
    if (!connected)
	return;

    cat.debug("Disconnecting from pool.");
    FPPool_Close(pool);
    connected = false;
}


void BlobFsMarshaller::checkForErrors(string action, ...) const
    throw(IoError)
{
    FPErrorInfo errorInfo;

    FPPool_GetLastErrorInfo(&errorInfo);

    if (errorInfo.error) {
	cat.error("Error %i(%i): %s %s", errorInfo.error, errorInfo.errorClass,
		  errorInfo.errorString, errorInfo.message);
	cat.error(" systemError: %i", errorInfo.systemError);
	cat.error(errorInfo.trace);

	throw IoError(EIO, "Can't %s: %s", action.c_str(), errorInfo.message);
    }
}

// File related methods

FileWriter& BlobFsMarshaller::loadFile(FileHandle fHandle) const
    throw(IoError)
{
    cat.trace("reading fHandle");

    size_t size = getFileSize(fHandle);
    FileWriter *data = new FileWriter(size);
    FPStreamRef streamRef = FPStream_CreateBufferForOutput(
	(char *) data->getData(), data->getSize());

    cat.debug("Reading file with size %uB", size);
    FPTag_BlobRead(fHandle.getTagRef(), streamRef, FP_OPTION_DEFAULT_OPTIONS);
    cat.debug("tagRef %lli - read", fHandle.getTagRef());
    checkForErrors("load file");
    data->seek(0);

    return *data;
}

FileHandle BlobFsMarshaller::storeFile(const DirHandle dirHandle,
				       const FileWriter& data)
    throw(IoError)
{
    connectPool();

    cat.trace("writing file data.");

    const char *bytes = static_cast<const char *>(data.getData());
    size_t size = data.getSize();
    FPTagRef topTag = FPClip_GetTopTag(dirHandle.getClipRef());
    FPTagRef fileTag = FPTag_Create(topTag, "file");
    // TODO NEW CODE
    FPTag_SetLongAttribute(fileTag, "size", size);
    // END NEW CODE
    FPStreamRef streamRef =
	FPStream_CreateBufferForInput(const_cast<char *>(bytes), size);

    FPTag_BlobWrite(fileTag, streamRef, FP_OPTION_SERVER_CALCID_STREAMING);
    checkForErrors("store file");
    cat.debug("tagRef %lli - stored", fileTag);

    return FileHandle(fileTag);
}

void BlobFsMarshaller::closeFileHandle(const FileHandle fHandle) const
    throw(IoError)
{
    FPTag_Close(fHandle.getTagRef());
    cat.debug("tagRef %lli - close", fHandle.getTagRef());
    checkForErrors("close file");
}

FileHandle BlobFsMarshaller::copyFileHandle(const DirHandle dirHandle,
					    const FileHandle fHandle) const
    throw(IoError)
{
    FPTagRef newTopTag = FPClip_GetTopTag(dirHandle.getClipRef());
    FPTagRef newTag =
	FPTag_Copy(fHandle.getTagRef(), newTopTag, FP_OPTION_COPY_BLOBDATA);

    cat.debug("tagRef %lli - copy of %lli for %lli",
	      newTag, fHandle.getTagRef(), dirHandle.getClipRef());
    checkForErrors("copy file");

    return FileHandle(newTag);
}

size_t BlobFsMarshaller::getFileSize(const FileHandle fHandle) const
    throw(IoError)
{
    // TODO NEW CODE
    size_t size = FPTag_GetLongAttribute(fHandle.getTagRef(), "size");
//    size_t size = FPTag_GetBlobSize(fHandle.getTagRef());
    // END TODO

    cat.debug("tag %lli -> size %zu", fHandle.getTagRef(), size);
    checkForErrors("get file size");
    return size;
}

void BlobFsMarshaller::setFileStringAttr(const FileHandle fHandle,
					const string key, const string value)
    const
    throw(IoError)
{
    FPTag_SetStringAttribute(fHandle.getTagRef(), key.c_str(), value.c_str());
}

string BlobFsMarshaller::getFileStringAttr(const FileHandle fHandle,
					   const string key) const
    throw(IoError)
{
    const FPInt initialSize = 80;
    FPInt size = initialSize;
    char *buf = static_cast<char *>(alloca(size));

    FPTag_GetStringAttribute(fHandle.getTagRef(), key.c_str(), buf, &size);

    if (size > initialSize) {
	buf = static_cast<char *>(alloca(size));
	FPTag_GetStringAttribute(fHandle.getTagRef(), key.c_str(), buf, &size);
    }

    checkForErrors("get file attribute <%s>", key.c_str());

    return buf;
}

void BlobFsMarshaller::setFileLongAttr(const FileHandle fHandle,
				       const string key, long value) const
    throw(IoError)
{
    FPTag_SetLongAttribute(fHandle.getTagRef(), key.c_str(), value);
}

long BlobFsMarshaller::getFileLongAttr(const FileHandle fHandle,
				       const string key) const
    throw(IoError)
{
    return FPTag_GetLongAttribute(fHandle.getTagRef(), key.c_str());
}

// RootDir related methods

const DirHandle BlobFsMarshaller::openDirHandle(const Uid uid)
    throw(IoError)
{
    connectPool();

    const FPClipID& clipID = uid.getClipID();
    const FPClipRef clipRef = FPClip_Open(pool, clipID, FP_OPEN_ASTREE);

    cat.debug("clipRef %lli - open %s", clipRef, clipID);
    checkForErrors("open dirHandle");
    assert(clipRef != 0);

    return DirHandle(clipRef);
}

const DirHandle BlobFsMarshaller::newDirHandle()
    throw(IoError)
{
    connectPool();

    FPClipRef clipRef = FPClip_Create(pool, "BlobFs clip");

    cat.debug("clipRef %lli - created", clipRef);
    checkForErrors("create dirHandle");
    return DirHandle(clipRef);
}

void BlobFsMarshaller::closeDirHandle(const DirHandle dirHandle) const
    throw(IoError)
{
    FPClipRef clipRef = dirHandle.getClipRef();

    FPTag_Close(FPClip_GetTopTag(clipRef));
    // TODO Fix issue where FPClip_Close() throws
//    FPClip_Close(clipRef);
    cat.debug("clipRef %lli - closed", dirHandle.getClipRef());
    checkForErrors("close dirHandle");
}

NodeHandleIterator BlobFsMarshaller::getHandleIterator(const DirHandle dirHandle)
    const
    throw(IoError)
{
    return NodeHandleIterator(*new BlobFsHandleIterator(dirHandle.getClipRef()));
}

const Uid BlobFsMarshaller::storeRootDir(const DirHandle dirHandle)
    throw(IoError)
{
    FPClipID clipID;

    FPClip_Write(dirHandle.getClipRef(), clipID);
    cat.debug("clipRef %lli - wrote %s", dirHandle.getClipRef(), clipID);

    Uid uid(clipID);

    rootUidMarshaller.storeRootUid(uid);
    return uid;
}
