#include "BlobfsGuard.hh"
#include "CompositeChecker.hh"
#include "DirChecker.hh"
#include "FileChecker.hh"
#include "ResultImpl.hh"

#include <iostream>

void checkAndAdd(CompositeChecker& composite, BlobfsGuard& guard,
		 Result& result, const Checker *checker)
{
    checker->check(guard.getRoot(), result);
    composite.addChecker(checker);
}

void phase1test(CompositeChecker& checker, Result& result) {
    BlobfsGuard guard;
    const FileChecker *file = new FileChecker(
	guard.getRoot(), "foo", "The quick brown fox jumps over the lazy dog");
    const DirChecker *dir = new DirChecker(guard.getRoot(), "bar");
    const FileChecker *file2 = new FileChecker(
	guard.getRoot(), "bar/foo", "This is a test.\n1 2 3\n4 5 6\n");

    sleep(5);

    checkAndAdd(checker, guard, result, file);
    checkAndAdd(checker, guard, result, dir);
    checkAndAdd(checker, guard, result, file2);
}

void phase2test(CompositeChecker& checker, Result& result) {
    BlobfsGuard guard;

    checker.check(guard.getRoot(), result);
}

int main(int argc, char **argv) {
    CompositeChecker checker;
    ResultImpl result;

    phase1test(checker, result);
    std::cout << "========================================" << std::endl;
    phase2test(checker, result);

    std::string msg = result.toString();

    if (msg.empty())
	std::cout << "All tests passed" << std::endl;
    else
	std::cout << msg;
}
