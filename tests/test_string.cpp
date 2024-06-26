#include <s2string.h>

#include <s2test.h>

void test_string()
{
	s2::test_group("string");

	s2::string strEmpty;
	S2_TEST(strEmpty.allocsize() == 0);
	S2_TEST(strEmpty.c_str() != nullptr);
	S2_TEST(strEmpty.len() == 0);
	S2_TEST(strEmpty.indexof('a') == -1);
	S2_TEST(strEmpty.lastindexof('a') == -1);
	S2_TEST(strEmpty == "");
	S2_TEST(strEmpty == nullptr);
	S2_TEST(strEmpty == strEmpty);
	S2_TEST(strEmpty.is_null());

	strEmpty = "";
	S2_TEST(strEmpty.c_str() != nullptr);
	S2_TEST(strEmpty.len() == 0);
	S2_TEST(strEmpty.is_null());

	strEmpty = "hi";
	strEmpty = "";
	S2_TEST(strEmpty.len() == 0);
	S2_TEST(strEmpty == "");
	S2_TEST(strEmpty.indexof('a') == -1);
	S2_TEST(strEmpty.lastindexof('a') == -1);
	S2_TEST(!strEmpty.is_null());

	s2::string str = "Hello";
	S2_TEST(!str.is_null());
	S2_TEST(str == "Hello");
	S2_TEST(str[0] == 'H');
	str[4] = '!';
	S2_TEST(str == "Hell!");
	S2_TEST(str.len() == 5);

	str = "Hello";
	S2_TEST(str == "Hello");
	str += " world";
	S2_TEST(str == "Hello world");

	S2_TEST(str.contains("world"));
	S2_TEST(!str.contains("WORLD"));

	S2_TEST(str.contains_nocase("hello"));
	S2_TEST(str.contains_nocase("WORLD"));
	S2_TEST(!str.contains_nocase("earth"));

	S2_TEST(str.startswith("Hello"));
	S2_TEST(!str.startswith("world"));
	S2_TEST(str.startswith("Hello world"));

	S2_TEST(str.endswith(" world"));
	S2_TEST(!str.endswith(" worlddd"));
	S2_TEST(str.endswith("Hello world"));

	S2_TEST(str.indexof('l') == 2);
	S2_TEST(str.indexof('!') == -1);
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

	s2::stringsplit parse3("a;b;c;", ";");
	S2_TEST(parse3.len() == 4);
	S2_TEST(parse3[3] == "");

	s2::stringsplit parse4(";a;b;c", ";");
	S2_TEST(parse4.len() == 4);
	S2_TEST(parse4[0] == "");
	S2_TEST(parse4[3] == "c");

	s2::stringsplit parse5("", ".");
	S2_TEST(parse5.len() == 1);

	s2::stringsplit parse6(".", ".");
	S2_TEST(parse6.len() == 2);

	s2::stringsplit parse7("hello \"world, this is me!\" and you", true);
	S2_TEST(parse7.len() == 4);
	S2_TEST(parse7[0] == "hello");
	S2_TEST(parse7[1] == "world, this is me!");
	S2_TEST(parse7[2] == "and");
	S2_TEST(parse7[3] == "you");

	s2::stringsplit parse8("", true);
	S2_TEST(parse8.len() == 0);

	str = "   \t\t foo\tbar\n\n  \r\n";
	S2_TEST(str.trim() == "foo\tbar");
	str = "   \t\t foo\tbar";
	S2_TEST(str.trim() == "foo\tbar");
	str = "foo\tbar\n\n  \r\n";
	S2_TEST(str.trim() == "foo\tbar");
	str = "foo\tbar";
	S2_TEST(str.trim() == "foo\tbar");

	S2_TEST(str.substr(4) == "bar");
	S2_TEST(str.substr(0, 3) == "foo");
	S2_TEST(str.substr(3, 1) == "\t");
	S2_TEST(str.substr(-3) == "bar");
	S2_TEST(str.substr(str.len()) == "");
	S2_TEST(str.substr(str.len() + 3) == "");
	S2_TEST(str.substr(0, str.len()) == "foo\tbar");
	S2_TEST(str.substr(0, str.len() + 3) == "foo\tbar");

	str = "helloworld";
	str.insert(", ", 5);
	S2_TEST(str == "hello, world");
	S2_TEST(str.len() == 12);

	str.remove(5, 2);
	S2_TEST(str == "helloworld");
	S2_TEST(str.len() == 10);

	s2::string hello = "hello";
	s2::string world = "world";
	s2::string helloworld = hello + " " + world;
	S2_TEST(helloworld == "hello world");
	S2_TEST(hello == "hello");
	S2_TEST(world == "world");

	str = "HELLO, " + world;
	S2_TEST(str == "HELLO, world");

	str = "this.is.a.test";
	str = str.replace('.', ' ');
	S2_TEST(str == "this is a test");
	S2_TEST(str.replace("", "a") == "this is a test");
	str = str.replace("a", "my");
	S2_TEST(str == "this is my test");
	str = "foo";
	str = str.replace("foo", "bar");
	S2_TEST(str == "bar");
	str = str.replace("bar", "foooo");
	S2_TEST(str == "foooo");
	str = str.replace("foooo", "b");
	S2_TEST(str == "b");
	S2_TEST(str.len() == 1);
	str = "a b-c";
	S2_TEST(str.replace(" -", '_') == "a_b_c");

	str = "12.3";
	S2_TEST(str.as_int() == 12);
	S2_TEST(str.as_large_int() == 12);
	S2_TEST(str.as_uint() == 12);
	S2_TEST(str.as_large_uint() == 12);
	S2_TEST(str.as_float() == 12.3f);
	S2_TEST(str.as_double() == 12.3);
	S2_TEST(str.as_bool() == true);

	s2::string strEmptySource;
	s2::string strEmpty2(strEmptySource);
	S2_TEST(strEmpty2.allocsize() == 0);
	strEmpty2.append("test");
	S2_TEST(strEmpty2 == "test");
}
