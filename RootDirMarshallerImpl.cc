#include "Config.hh"
#include "NodeAttrHelper.hh"
#include "ProcessUserContext.hh"
#include "RootDirMarshallerImpl.hh"
#include "StringUtils.hh"

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <log4cpp/Category.hh>
#include <map>
#include <string>

using std::string;

// TODO Merge RootDirMarshallerImpl & ChildFactory into NodeMarshaller?

log4cpp::Category& RootDirMarshallerImpl::cat =
    log4cpp::Category::getInstance("RootDirMarshallerImpl");

RootDirMarshallerImpl::RootDirMarshallerImpl(const fs::path path)
    throw()
    : path(path) { }

void RootDirMarshallerImpl::marshal(const RootDir& rootDir, const Uid& uid)
    const
    throw(IoError)
{
    const NodeAttr& attr = rootDir.getAttr();
    Config config(path);

    config["rootdir.clip"] = uid.getClipID();
    config["rootdir.mode"] = boost::lexical_cast<std::string>(attr.getMode());
    config["rootdir.uid"] = attr.getUid();
    config["rootdir.gid"] = attr.getGid();
    cat.debug("Persisting RootDir mode: %lo, address = %p", attr.getMode(),
	      &attr);

    config.store();
}

RootDir& RootDirMarshallerImpl::unmarshal(FsMarshaller& marshaller) const
    throw(IoError)
{
    Config config(path);

    config.load();

    string clipId = config["rootdir.clip"];

    if (! clipId.empty()) {
	RootDir& rootDir = *new RootDir(marshaller, *this, Uid(clipId.c_str()));
	string uid = config["rootdir.uid"];
	string gid = config["rootdir.gid"];
	string mode = config["rootdir.mode"];
	NodeAttr& attr = rootDir.getAttr();
	NodeAttrHelper attrHelper;

	attr.setUid(parse<uid_t>(uid));
	attr.setGid(parse<gid_t>(gid));
	attrHelper.setPerms(attr, parse<mode_t>(mode));
	// TODO Remove
//	attr.setMode(parse<mode_t>(mode));
	cat.debug("RootDir has mode: %lo, address = %p", attr.getMode(), &attr);

	return rootDir;
    } else {
	RootDir& rootDir = *new RootDir(marshaller, *this);
	NodeAttr& attr = rootDir.getAttr();
	ProcessUserContext userContext;
	NodeAttrHelper attrHelper;

	attr.setUid(userContext.getUid());
	attr.setGid(userContext.getGid());
	attrHelper.setPerms(attr, userContext.getMode());

	return rootDir;
    }
}
