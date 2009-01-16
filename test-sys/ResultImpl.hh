#ifndef _RESULTIMPL_HH_
#define _RESULTIMPL_HH_

#include "Result.hh"

#include <vector>

class Failure {

private:
    std::string file;
    int line;
    std::string msg;

public:
    Failure(std::string file, int line, std::string msg)
	throw();

    std::string toString() const
	throw();

};

class ResultImpl : public Result {

private:
    std::vector<Failure *> failures;

public:
    void reportFailure(std::string file, int line, std::string msg)
	throw();

    std::string toString() const
	throw();

    ~ResultImpl()
	throw();

};

#endif /* _RESULTIMPL_HH_ */
