#include <s2stringpath.h>

#include <s2test.h>

void test_stringpath()
{
	s2::test_group("stringpath");

	S2_TEST(s2::path::getExtension("hello.txt") == ".txt");
	S2_TEST(s2::path::getExtension("hello/test/example.txt") == ".txt");
	S2_TEST(s2::path::getExtension("hello/test.zip/example.txt") == ".txt");
	S2_TEST(s2::path::getExtension("C:\\Users\\Scratch2\\Desktop\\hello/test/example.txt") == ".txt");
	S2_TEST(s2::path::getExtension("hello") == "");
	S2_TEST(s2::path::getExtension("hello.") == ".");
	S2_TEST(s2::path::getExtension("") == "");

	S2_TEST(s2::path::hasExtension("hello.txt"));
	S2_TEST(s2::path::hasExtension("hello."));
	S2_TEST(!s2::path::hasExtension("hello"));
	S2_TEST(!s2::path::hasExtension("test.zip/hello"));
	S2_TEST(!s2::path::hasExtension(""));

	S2_TEST(s2::path::changeExtension("hello.txt", "png") == "hello.png");
	S2_TEST(s2::path::changeExtension("hello.", ".png") == "hello.png");
	S2_TEST(s2::path::changeExtension("hello", "png") == "hello.png");
	S2_TEST(s2::path::changeExtension("hello", ".png") == "hello.png");
	S2_TEST(s2::path::changeExtension("test.zip/hello.txt", "png") == "test.zip/hello.png");
	S2_TEST(s2::path::changeExtension("", "zip") == ".zip");
	S2_TEST(s2::path::changeExtension("", "") == "");

	S2_TEST(s2::path::removeExtension("hello.txt") == "hello");
	S2_TEST(s2::path::removeExtension("hello.") == "hello");
	S2_TEST(s2::path::removeExtension("hello") == "hello");
	S2_TEST(s2::path::removeExtension("test.zip/hello.txt") == "test.zip/hello");
	S2_TEST(s2::path::removeExtension("test.zip/hello.") == "test.zip/hello");
	S2_TEST(s2::path::removeExtension("test.zip/hello") == "test.zip/hello");
	S2_TEST(s2::path::removeExtension("") == "");

	S2_TEST(s2::path::join("foo", "bar") == "foo/bar");
	S2_TEST(s2::path::join("foo/", "bar") == "foo/bar");
	S2_TEST(s2::path::join("foo", "/bar") == "foo/bar");
	S2_TEST(s2::path::join("foo/", "") == "foo/");
	S2_TEST(s2::path::join("foo", "") == "foo/");
	S2_TEST(s2::path::join("", "foo") == "/foo");
	S2_TEST(s2::path::join("", "/foo") == "/foo");
	S2_TEST(s2::path::join("", "") == "");

	S2_TEST(s2::path::equals("foo/bar/hello.txt", "foo\\bar\\hello.txt", true));
	S2_TEST(s2::path::equals("foo/bar/Hello.txt", "foo\\bar\\hello.txt", false));
	S2_TEST(!s2::path::equals("foo/bar/Helloo.txt", "foo\\bar\\hello.txt", false));
	S2_TEST(!s2::path::equals("foobar/Hello.txt", "foo\\bar\\Hello.txt", true));
	S2_TEST(s2::path::equals("", "", true));

	S2_TEST(s2::path::getDirectoryName("hello/world/foo.txt") == "hello/world/");
	S2_TEST(s2::path::getDirectoryName("hello/world/") == "hello/world/");
	S2_TEST(s2::path::getDirectoryName("hello/world") == "hello/");
	S2_TEST(s2::path::getDirectoryName("hello/") == "hello/");
	S2_TEST(s2::path::getDirectoryName("hello") == "");
	S2_TEST(s2::path::getDirectoryName("") == "");
}
