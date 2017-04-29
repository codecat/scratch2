#include <s2ref.h>

#include <s2test.h>

#include "structs.h"

void test_ref()
{
	s2::test_group("ref");

	S2_TEST(_numFooInstances == 0);
	{
		s2::ref<Foo> ref;
		S2_TEST(_numFooInstances == 0);
		ref = new Foo;
		S2_TEST(ref->num == 0);
		S2_TEST(_numFooInstances == 1);
		S2_TEST(ref.count() == 1);
		{
			s2::ref<Foo> ref2 = ref;
			S2_TEST(ref.count() == 2);
			S2_TEST(ref2.count() == 2);
			S2_TEST(ref.ptr() == ref2.ptr());
			S2_TEST(_numFooInstances == 1);
			ref2 = nullptr;
			S2_TEST(ref.count() == 1);
			S2_TEST(ref2.ptr() == nullptr);
			S2_TEST(ref2.count() == 0);
		}
		S2_TEST(ref.count() == 1);
		S2_TEST(_numFooInstances == 1);
	}
	S2_TEST(_numFooInstances == 0);
}
