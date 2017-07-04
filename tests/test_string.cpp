#include <s2string.h>

#include <s2test.h>

void test_string()
{
	s2::test_group("string");

	s2::string str = "Hello";
	S2_TEST(str == "Hello");
	S2_TEST(str[0] == 'H');
	str[4] = '!';
	S2_TEST(str == "Hell!");
	S2_TEST(str.len() == 5);

	str = "Hello";
	S2_TEST(str == "Hello");
	str += " world";
	S2_TEST(str == "Hello world");

	S2_TEST(str.indexof("world") == 6);
	S2_TEST(str.indexof("world!") == -1);

	str.setf("Test");
	S2_TEST(str == "Test");
	str.setf("Test %d", 10);
	S2_TEST(str == "Test 10");
	str.appendf(", and the other test is %f", 3.14159f);
	S2_TEST(str == "Test 10, and the other test is 3.141590");

	str = s2::strprintf("Test %d %d %d", 10, 20, 30);
	S2_TEST(str == "Test 10 20 30");

	s2::stringsplit parse(str, " ");
	S2_TEST(parse.len() == 4);
	S2_TEST(parse[0] == "Test");
	S2_TEST(parse[1] == "10");
	S2_TEST(parse[2] == "20");
	S2_TEST(parse[3] == "30");

	s2::stringsplit parse2("this is a test", " ", 2);
	S2_TEST(parse2.len() == 2);
	S2_TEST(parse2[0] == "this");
	S2_TEST(parse2[1] == "is a test");

	str = "   \t\t foo\tbar\n\n  \r\n";
	S2_TEST(str.trim() == "foo\tbar");
	str = "   \t\t foo\tbar";
	S2_TEST(str.trim() == "foo\tbar");
	str = "foo\tbar\n\n  \r\n";
	S2_TEST(str.trim() == "foo\tbar");
	str = "foo\tbar";
	S2_TEST(str.trim() == "foo\tbar");
}
