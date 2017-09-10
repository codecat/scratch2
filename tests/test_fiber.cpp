#include <s2fiber.h>

#include <s2test.h>

#include <cstdio>

void test_fiber()
{
	s2::test_group("fiber");

	{
		s2::fiber fib([](s2::fiber &f) {
			while (true) {
				printf("fiber call\n");
				f.yield();
				printf("fiber ret\n");
			}
		});
		while (true) {
			printf("going to fib..\n");
			fib.resume();
			printf("returned from fib..\n");
		}
	}

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
		int x = 0;
		s2::fiber fib([](s2::fiber &f) {
			for (int i = 0; i < 10; i++) {
				f.yield();
			}
		});
		for (int i = 0; i < 10; i++) {
			fib.resume();
			S2_TEST(x == i + 1);
		}
		fib.resume();
		S2_TEST(fib.isfinished());
	}
}
