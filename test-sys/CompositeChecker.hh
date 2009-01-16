#ifndef _COMPOSITECHECKER_HH_
#define _COMPOSITECHECKER_HH_

#include "Result.hh"
#include "Checker.hh"

#include <boost/filesystem.hpp>
#include <vector>

class CompositeChecker : public Checker {

private:
    std::vector<const Checker *> checkers;

public:
    void check(boost::filesystem::path root, Result& result) const
	throw();

    void addChecker(const Checker *checker)
	throw();

};

#endif /* _COMPOSITECHECKER_HH_ */
