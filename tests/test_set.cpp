#include <s2set.h>

#include <s2test.h>

#include <s2string.h>

void test_set()
{
	s2::test_group("set");

	s2::set<s2::string> set;
	set.add("hello");
	set.add("world");
	S2_TEST(set.len() == 2);
	S2_TEST(set.contains("hello"));
	S2_TEST(set.contains("world"));
	S2_TEST(!set.contains("foo"));

	set.remove("hello");
	S2_TEST(set.len() == 1);
	S2_TEST(!set.contains("hello"));
	S2_TEST(set.contains("world"));
	S2_TEST(set.index_of("world") == 0);

	set.clear();
	S2_TEST(set.len() == 0);
	S2_TEST(!set.contains("world"));
	S2_TEST(set.index_of("world") == -1);
}
