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

	str.setf("Test");
	S2_TEST(str == "Test");
	str.setf("Test %d", 10);
	S2_TEST(str == "Test 10");
	str.appendf(", and the other test is %f", 3.14159f);
	S2_TEST(str == "Test 10, and the other test is 3.141590");
}
