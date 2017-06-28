#include <s2func.h>

#include <s2test.h>

void test_func()
{
	s2::test_group("func");

	s2::func<void()> func;
	S2_TEST(func == nullptr);

	int n = 0;
	func = [&n]() { n = 10; };
	S2_TEST(n == 0);
	func();
	S2_TEST(n == 10);
}
