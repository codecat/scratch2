#include <s2list.h>

#include <s2test.h>

#include "structs.h"

void test_list()
{
	s2::test_group("list");

	s2::list<int> arr;
	arr.add() = 10;
	arr.add() = 20;
	arr.add(30);
	S2_TEST(arr.len() == 3);
	S2_TEST(arr[1] == 20);
	arr[2] = 50;
	S2_TEST(arr[2] == 50);

	s2::list<int> arr2(arr);
	S2_TEST(arr2.len() == 3);
	S2_TEST(&arr[0] != &arr2[0]);
	S2_TEST(arr[0] == arr2[0]);
	arr2.clear();
	S2_TEST(arr2.len() == 0);
	S2_TEST(arr[1] == 20);

	arr.clear();
	S2_TEST(arr.len() == 0);

	{
		s2::list<Foo> foo_arr;
		Foo &f = foo_arr.add();
		f.num = 10;
		S2_TEST(_numFooInstances == 1);

		s2::list<Foo> foo_arr2(foo_arr);
		S2_TEST(_numFooInstances == 2);
		foo_arr2.clear();
		S2_TEST(_numFooInstances == 1);

		foo_arr.remove(0);
		S2_TEST(_numFooInstances == 0);

		Foo f2;
		f2.num = 20;
		S2_TEST(_numFooInstances == 1);
		foo_arr.add(f2);
		S2_TEST(_numFooInstances == 2);
		for (Foo &f : foo_arr) {
			S2_TEST(f.num == 20);
		}
		S2_TEST(foo_arr.begin() != foo_arr.end());
		foo_arr.clear();
		S2_TEST(_numFooInstances == 1);
		S2_TEST(foo_arr.begin() == foo_arr.end());
	}

	S2_TEST(_numFooInstances == 0);
}
