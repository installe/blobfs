#include "filehandle.hh"

DirHandle::DirHandle(FPClipRef fpClipRef) : fpClipRef(fpClipRef) { }

FPClipRef DirHandle::getClipRef() const { return fpClipRef; }

FileHandle::FileHandle(FPTagRef fpTagRef) : fpTagRef(fpTagRef) { }

FPTagRef FileHandle::getTagRef() const { return fpTagRef; }
