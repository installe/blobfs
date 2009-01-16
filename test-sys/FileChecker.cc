#include "FileChecker.hh"
#include "Result.hh"

#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>

template <class T> void checkEquals(Result& result,
				    boost::filesystem::path& path, 
				    T expected, T actual,
				    const std::string attrName)
    throw()
{
    assertEquals<T>(result, expected, actual,
		    attrName + " of file <" + path.string() + ">");
}

FileChecker::FileChecker(boost::filesystem::path root,
			 boost::filesystem::path relPath, std::string content)
    throw()
    : NodeChecker(relPath, 0644), content(content)
{
    write(root, content);
}

void FileChecker::check(boost::filesystem::path root, Result& result) const
    throw()
{
    NodeChecker::check(root, result);

    struct stat st;
    boost::filesystem::path path = root / getRelPath();

    int res = stat(path.string().c_str(), &st);

    assertEquals(result, 0, res, "Stat failed");
    checkEquals<off_t>(result, path, content.size(), st.st_size, "size");
    checkEquals<mode_t>(result, path, S_IFREG, st.st_mode & S_IFMT, "mode");
    assertEquals(result, content, read(root));
}

void FileChecker::write(boost::filesystem::path root, std::string content) const
    throw()
{
    boost::filesystem::path path = root / getRelPath();
    boost::filesystem::ofstream ofs(path);

    ofs << content;
    ofs.close();
}

std::string FileChecker::read(boost::filesystem::path root) const
    throw()
{
    boost::filesystem::path path = root / getRelPath();
    boost::filesystem::ifstream ifs(path, std::ios_base::binary);
    std::ostringstream os;

    while (!ifs.bad() && !ifs.eof()) {
	char buf[256];
	int numRead = ifs.readsome(buf, sizeof(buf) - 1);

	if (numRead <= 0)
	    break;

	os.write(buf, numRead);
    }

    return os.str();
}
