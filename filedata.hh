#ifndef _FILEDATA_HH_
#define _FILEDATA_HH_

#include <log4cpp/Category.hh>

#include <exception>
#include <new>

using namespace std;

class FileWriter {

private:
    const static int BUFSIZE = 4096;

    static log4cpp::Category& cat;

    size_t size;
    size_t offset;

    size_t capacity;
    void *data;

    void setSize(size_t _size) throw();

public:
    FileWriter(size_t size) throw(bad_alloc);

    FileWriter() throw(bad_alloc);

    ~FileWriter() throw();

    void seek(size_t offset) throw();

    size_t read(void *buf, size_t size) throw();

    size_t write(const void *buf, size_t size) throw();

    const void *getData() const throw();

    size_t getSize() const throw();

};

#endif /* _FILEDATA_HH_ */
