#include <s2dict.h>

#include <s2test.h>

#include <s2string.h>
#include "structs.h"

void test_dict()
{
	s2::test_group("dict");

	s2::dict<s2::string, s2::string> dict;
	dict["foo"] = "FOO";
	dict["bar"] = "BAR";
	S2_TEST(dict.len() == 2);
	S2_TEST(dict["foo"] == "FOO");
	S2_TEST(dict["bar"] == "BAR");
	for (auto &pair : dict) {
		pair.value() = "FUBAR";
	}
	S2_TEST(dict["foo"] == "FUBAR");
	S2_TEST(dict["bar"] == "FUBAR");
	dict.remove("foo");
	S2_TEST(dict.index_of("bar") == 0);
	S2_TEST(dict.index_of("foo") == -1);
	dict.clear();
	S2_TEST(dict.len() == 0);

	{
		s2::dict<int, Foo> dict_foo;
		S2_TEST(_numFooInstances == 0);
		dict_foo.add(10).value().num = 20;
		S2_TEST(_numFooInstances == 1);
		dict_foo.add(20).value().num = 40;
		S2_TEST(_numFooInstances == 2);
		S2_TEST(dict_foo[10].num == 20);
		S2_TEST(dict_foo[20].num == 40);
		dict_foo.remove_at(0);
		S2_TEST(_numFooInstances == 1);
	}

	S2_TEST(_numFooInstances == 0);
}
