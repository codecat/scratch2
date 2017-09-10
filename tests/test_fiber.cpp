#include <s2fiber.h>

#include <s2test.h>

#include <cstdio>

static int _x = 0;

void test_fiber()
{
	s2::test_group("fiber");

	{
		s2::fiber fib([](s2::fiber &f) { });
		fib.resume();
		S2_TEST(fib.isfinished());
	}

	{
		s2::fiber fib([](s2::fiber &f) { f.yield(); });
		fib.resume();
		S2_TEST(!fib.isfinished());
		fib.resume();
		S2_TEST(fib.isfinished());
	}

	{
		s2::fiber fib([](s2::fiber &f) {
			for (int i = 0; i < 3; i++) {
				_x = i;
				f.yield();
			}
		});
		for (int i = 0; i < 3; i++) {
			fib.resume();
			S2_TEST(_x == i);
		}
		fib.resume();
		S2_TEST(fib.isfinished());
	}
}
