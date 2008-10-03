#include "filedata.hh"

#include <exception>
#include <string>

#include <stdlib.h>
#include <string.h>

using namespace std;

log4cpp::Category& FileWriter::cat =
    log4cpp::Category::getInstance("FileWriter");

FileWriter::FileWriter(size_t size)
    throw(bad_alloc)
    : size(size), offset(0), capacity(size)
{
    data = malloc(capacity);

    if (data == NULL)
	throw bad_alloc();

    cat.debug("FileWriter: %p alloc %iB", data, capacity);
}

FileWriter::FileWriter()
    throw(bad_alloc)
    : size(0), offset(0), capacity(BUFSIZE)
{
    data = malloc(capacity);

    if (data == NULL)
	throw bad_alloc();

    cat.debug("FileWriter: %p alloc %iB", data, capacity);
}

FileWriter::~FileWriter()
    throw()
{
    cat.debug("FileWriter: buf %p free", data);
    free(data);
}

void FileWriter::setSize(size_t _size)
    throw()
{
    size = _size;
}

void FileWriter::seek(size_t _offset)
    throw()
{
    cat.debug("FileWriter: buf %p seek to %i", data, _offset);
    if (_offset > size)
	setSize(_offset);

    // TODO grow to boundaries of BUFSIZE
    if (_offset > capacity) {
	void *olddata = data;

	capacity = _offset + BUFSIZE;
	data = realloc(olddata, capacity);
	cat.debug("FileWriter: buf %p -> %p realloc %iB",
		  olddata, data, capacity);
    }

    offset = _offset;
}

size_t FileWriter::write(const void *buf, size_t _size)
    throw()
{
    if (_size <= 0)
	return 0;

    size_t end = offset + _size;

    if (end > size)
	setSize(end);

    if (end > capacity) {
	void *olddata = data;

	capacity = end + BUFSIZE;
	cat.debug( "FileWriter: buf %p -> %p realloc %iB",
		   olddata, data, capacity);
	data = realloc(olddata, capacity);
    }

    memcpy((char *) data + offset, buf, _size);

    offset = end;

    return _size;
}

size_t FileWriter::read(void *buf, size_t _size)
    throw()
{
    if (_size <= 0)
	return 0;

    size_t end = min(offset + _size, size);
    size_t numRead = end - offset;

    cat.debug("FileWriter: buf %p read %iB @ offset %u", data, _size, offset);
    memcpy(buf, (char *) data + offset, numRead);
    offset = end;

    return numRead;
}

const void *FileWriter::getData() const
    throw()
{
    return data;
}

size_t FileWriter::getSize() const
    throw()
{
    return size;
}
