#ifndef _CONFIGITEM_HH_
#define _CONFIGITEM_HH_

#include <boost/lexical_cast.hpp>
#include <string>

using boost::bad_lexical_cast;
using boost::lexical_cast;
using std::string;

class ConfigItem {

private:
    string& value;

public:
    ConfigItem(string& value)
	throw() : value(value) { }

    template<class S> ConfigItem operator=(S v)
	throw(bad_lexical_cast)
    {
	value = lexical_cast<string>(v);
	return *this;
    }

    template<class S> operator S()
	throw(bad_lexical_cast)
    {
	return lexical_cast<S>(value);
    }

    operator bool()
	throw()
    {
	return ! value.empty();
    }

    friend bool operator==(ConfigItem item, string s) throw();

    friend bool operator==(string s, ConfigItem item) throw();

};

inline bool operator==(ConfigItem item, string s)
    throw()
{
    return s == item;
}

inline bool operator==(string s, ConfigItem item)
    throw()
{
    return s == item.value;
}

#endif /* _CONFIGITEM_HH_ */
