#ifndef _FILECHECKER_HH_
#define _FILECHECKER_HH_

#include "NodeChecker.hh"
#include "Result.hh"

#include <sys/types.h>

#include <boost/filesystem.hpp>
#include <string>

class FileChecker : public NodeChecker {

private:
    std::string content;

    void write(boost::filesystem::path root, std::string content) const
	throw();

    std::string read(boost::filesystem::path root) const
	throw();

public:
    FileChecker(boost::filesystem::path root, boost::filesystem::path relPath,
		std::string content)
	throw();

    void check(boost::filesystem::path root, Result& result) const
	throw();

};

#endif /* _FILECHECKER_HH_ */
