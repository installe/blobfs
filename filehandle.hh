#ifndef _FILEHANDLE_HH_
#define _FILEHANDLE_HH_

#include <FPAPI.h>

class FileHandle {

private:
     FPTagRef fpTagRef;

public:
     FileHandle(FPTagRef fpTagRef);

     FPTagRef getTagRef() const;

};

class DirHandle {

private:
     FPClipRef fpClipRef;

public:
     DirHandle(FPClipRef fpClipRef);

     FPClipRef getClipRef() const;
};

#endif /* _FILEHANDLE_HH_ */
