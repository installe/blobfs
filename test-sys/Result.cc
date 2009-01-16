#include "Result.hh"

void assertTrue(Result& result, bool b, const std::string msg) {
    if (! b) {
	result.reportFailure("", -1,
			     msg == "" ? "Expected true but was false" : msg);
    }
}
