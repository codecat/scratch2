#include <s2hashtable.h>

#include <s2test.h>

#include <s2string.h>
#include "structs.h"

void test_hashtable()
{
	s2::test_group("hashtable");

	s2::hashtable<s2::string, s2::string> dict;
	S2_TEST(dict.len() == 0);
	S2_TEST(dict.index_of("foo") == -1);
	dict["foo"] = "FOO";
	S2_TEST(dict.index_of("foo") == 0);
	dict["bar"] = "BAR";
	S2_TEST(dict.len() == 2);
	S2_TEST(dict["foo"] == "FOO");
	S2_TEST(dict["bar"] == "BAR");

	S2_TEST_MUST_THROW_AND_EQUAL(dict.add("bar"), s2::hashtableexception, s2::hashtableexception::duplicate_key);
	S2_TEST(dict.len() == 2);

	for (auto &pair : dict) {
		pair.value = "FUBAR";
	}
	S2_TEST(dict["foo"] == "FUBAR");
	S2_TEST(dict["bar"] == "FUBAR");
	dict.remove("foo");
	S2_TEST(dict.index_of("bar") == 0);
	S2_TEST(dict.index_of("foo") == -1);
	dict.clear();
	S2_TEST(dict.len() == 0);

	{
		s2::hashtable<int, Foo> dict_foo;
		S2_TEST(_numFooInstances == 0);
		dict_foo.add(10).num = 20;
		S2_TEST(_numFooInstances == 1);
		dict_foo.add(20).num = 40;
		S2_TEST(_numFooInstances == 2);
		S2_TEST(dict_foo[10].num == 20);
		S2_TEST(dict_foo[20].num == 40);
		dict_foo.remove_at(0);
		S2_TEST(_numFooInstances == 1);
	}

	S2_TEST(_numFooInstances == 0);

	s2::hashtable<int, Foo> dict_unsorted;
	dict_unsorted.add_unsorted(10);
	dict_unsorted.add_unsorted(5);
	S2_TEST(dict_unsorted.len() == 2);
	S2_TEST(dict_unsorted.at(0).hash == 10);
	dict_unsorted.sort();
	S2_TEST(dict_unsorted.at(0).hash == 5);
}
