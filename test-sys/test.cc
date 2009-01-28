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
    boost::filesystem::path root = guard.getRoot();
    const FileChecker *file1 = new FileChecker(
	root, "foo", "The quick brown fox jumps over the lazy dog");
    const DirChecker *dir = new DirChecker(root, "bar");
    const FileChecker *file2 = new FileChecker(
	root, "bar/foo", "This is a test.\n1 2 3\n4 5 6\n");
    FileChecker *file3 = new FileChecker(root, "test", "This is a test.");
    DirChecker *dir2 = new DirChecker(root, "bar/bar");
    FileChecker *file4 = new FileChecker(root, "bar/bar/foo", "1 2 3 test.\n");

    file3->setCTime(root, 42);
    file3->setATime(root, 666);
    dir2->setUid(root, 42);
    dir2->setGid(root, 42);
    file4->setMode(root, 600);

    sleep(5);

    checkAndAdd(checker, guard, result, file1);
    checkAndAdd(checker, guard, result, dir);
    checkAndAdd(checker, guard, result, file2);
    checkAndAdd(checker, guard, result, file3);
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
