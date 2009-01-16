#include "CompositeChecker.hh"

#include <algorithm>

class CheckRunner {

private:
    boost::filesystem::path root;
    Result& result;

public:
    CheckRunner(boost::filesystem::path root, Result& result)
	throw()
	: root(root), result(result) { }

    void operator()(const Checker *checker) const
	throw()
    {
	checker->check(root, result);
    }

};

void CompositeChecker::check(boost::filesystem::path root, Result& result) const
    throw()
{
    CheckRunner checkRunner(root, result);

    for_each(checkers.begin(), checkers.end(), checkRunner);
}

void CompositeChecker::addChecker(const Checker *checker)
    throw()
{
    checkers.push_back(checker);
}
