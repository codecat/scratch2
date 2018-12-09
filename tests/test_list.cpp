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
	S2_TEST(arr.indexof(20) == 1);
	S2_TEST(arr.indexof(15) == -1);
	S2_TEST(arr.contains(20));
	S2_TEST(!arr.contains(15));

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

	s2::list<int> rem_arr;
	rem_arr.add(0);
	rem_arr.add(1);
	rem_arr.add(2);
	rem_arr.add(3);
	rem_arr.add(4);
	S2_TEST(rem_arr[0] == 0);
	S2_TEST(rem_arr.len() == 5);
	rem_arr.remove(0);
	S2_TEST(rem_arr[0] == 1);
	S2_TEST(rem_arr.len() == 4);
	rem_arr.remove(2);
	S2_TEST(rem_arr[2] == 4);
	S2_TEST(rem_arr[0] == 1);
	rem_arr.remove(2);
	S2_TEST(rem_arr.len() == 2);
	S2_TEST(rem_arr[1] == 2);

	s2::list<int> inst_arr = {
		1, 2, 3, 4, 5
	};
	S2_TEST(inst_arr.len() == 5);
	S2_TEST(inst_arr[0] == 1);

	inst_arr = { 5, 4, 3, 2 };
	S2_TEST(inst_arr.len() == 4);
	S2_TEST(inst_arr[0] == 5);

	s2::list<int> stack;
	stack.push() = 5;
	stack.push() = 10;
	stack.push() = 15;
	S2_TEST(stack.len() == 3);
	S2_TEST(stack.top() == 15);
	S2_TEST(stack.pop() == 15);
	S2_TEST(stack.pop() == 10);
	S2_TEST(stack.top() == 5);

	arr.add(0);
	arr.add(1);
	arr.add(2);
	arr.add(3);
	arr.insert(1, 100);
	S2_TEST(arr[0] == 0);
	S2_TEST(arr[1] == 100);
	arr.insert(3, 200);
	S2_TEST(arr[2] == 1);
	S2_TEST(arr[3] == 200);
	arr.insert(6, 400);
	S2_TEST(arr[5] == 3);
	S2_TEST(arr[6] == 400);
}
