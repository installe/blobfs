#ifndef _CONFIG_HH_
#define _CONFIG_HH_

#include "ConfigItem.hh"
#include "InvalidArgumentError.hh"
#include "IoError.hh"
#include "LineParser.hh"
#include "UserInfo.hh"

#include <unistd.h>
#include <sys/types.h>

#include <boost/filesystem/fstream.hpp>
#include <map>

using boost::filesystem::ifstream;
using boost::filesystem::ofstream;
using std::map;

namespace fs = boost::filesystem;
using boost::filesystem::path;

class ConfigWriter {

private:
    fs::ofstream& ofs;

public:
    ConfigWriter(fs::ofstream& ofs)
	throw() : ofs(ofs) { }

    void operator()(const pair<string, string> entry)
	throw(IoError)
    {
	ofs << entry.first << " = " << entry.second << endl;
    }

};

class Config {

private:
    fs::path fName;
    map<string, string> properties;

    void parse(const string line)
	throw()
    {
	LineParser parser(line);

	properties[parser.getKey()] = parser.getVal();
    }

    fs::path parent(fs::path file) const
	throw()
    {
	fs::path parent;

	for (fs::path::iterator it = file.begin(); it != --file.end(); it++)
	    parent /= *it;

	return parent;
    }

public:
    Config(fs::path fName)
	throw(InvalidArgumentError)
	: fName(fs::complete(fName, UserInfo(getuid()).home()))
    {
	fs::create_directories(parent(fName));
    }

    void load()
	throw(IoError)
    {
	fs::ifstream ifs(fName);

	while (ifs) {
	    char buf[128];

	    ifs.getline(buf, sizeof(buf), '\n');
	    parse(buf);
	}

	cout.flush();
    }

    void store() const
	throw(IoError)
    {
	fs::ofstream ofs(fName);

	if (! ofs)
	    throw IoError(0, "bla");

	for_each(properties.begin(), properties.end(), ConfigWriter(ofs));
	ofs.flush();
	ofs.close();
    }

    ConfigItem operator[](string key)
	throw()
    {
	return ConfigItem(properties[key]);
    }

};


#endif /* _CONFIG_HH_ */
