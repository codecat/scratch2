# s2

Scratch2 is a collection of minimal single-header libraries that implement base functionality. All header files can be included individually, the headers do not depend on each other.

* Absolute core:
  * [`s2string.h`](#s2stringh)
  * [`s2list.h`](#s2listh)
  * [`s2dict.h`](#s2dicth)
  * [`s2ref.h`](#s2refh)
  * [`s2func.h`](#s2func)
* System utility:
  * [`s2file.h`](#s2fileh)
  * [`s2fiber.h`](#s2fiberh)
* Miscellaneous:
  * [`s2test.h`](#s2testh)

To use any of these, you need to define `S2_IMPL` in *one* implementation file and include the files you need there. Note that this does not apply to some files where there is a generic implementation and therefore must be used purely as a header, for example `s2list.h`.

## `s2string.h`

Provides the class `s2::string` to use as a normal string container. The most basic example would be:

```c++
#include <cstdio>
#include <s2string.h>

int main()
{
	s2::string test;
	test = "Hello, ";
	test += "world.";
	printf("%s\n", test.c_str());

	return 0;
}
```

## `s2list.h`

Provides the class `s2::list<T>` to use as a container of multiple elements. The most basic example would be:

```c++
#include <cstdio>
#include <s2list.h>

int main()
{
	s2::list<int> test;
	test.add(1);
	test.add(2);
	test.add(3);

	for (int num : test) {
		printf("%d\n", num);
	}

	return 0;
}
```

When using non-pointer type classes for `T`, be advised that when calling `add(const T &)`, you are invoking the copy constructor. To avoid calling the copy constructor needlessly, you can also call `add()` without a parameter, which will add a new element using the default (empty) constructor, and return the instance.

When such items are removed from the list, the destructor will be called. Indeed, `s2::list` manages its own available memory for each element. This means that it's illegal to get a reference to an element and then proceed to remove it from the list.

## `s2dict.h`

Provides the class `s2::dict<TKey, TValue>` to use as a container of key/value pairs. The most basic example would be:

```c++
#include <cstdio>
#include <s2dict.h>

int main()
{
	s2::dict<int, int> test;
	test[10] = 100;
	test[20] = 200;
	test[30] = 300;
	printf("%d, %d, %d\n", test[10], test[20], test[30]);

	return 0;
}
```

Read the note above about non-pointer type classes for `s2list.h`, as this also applies to this class. The only difference here is that it is applied to both the key and the value.

## `s2ref.h`

Provides the class `s2::ref<T>` to use as a reference counted pointer. The most basic example would be:

```c++
#include <cstdio>
#include <s2ref.h>

struct Foo {};

int main()
{
	s2::ref<Foo> test;
	{
		s2::ref<Foo> test2 = new Foo;
		test = test2;
	}
	printf("%d @ %p\n", test.count(), test.ptr());

	return 0;
}
```

## `s2func.h`

Provides a container for executable functions. The most basic example would be:

```c++
#include <cstdio>
#include <s2func.h>

int main()
{
	int num = 0;
	s2::func<void()> func = [&num]() {
		num += 10;
	};

	for (int i = 0; i < 10; i++) {
		func();
	}

	printf("num = %d\n", num);
	return 0;
}
```

## `s2file.h`

Provides the class `s2::file` to use for primitive reading and writing to files on disk. The most basic example would be:

```c++
#include <s2file.h>

int main()
{
	int number = 10;

	s2::file test("test.bin");
	test.open(s2::filemode::write);
	test.write(&number, sizeof(number));
	test.close();

	return 0;
}
```

Also included are the following functions:

```c++
bool s2::file_exists(const char* filename);
size_t s2::file_size(const char* filename);
```

## `s2fiber.h`

Provides the class `s2::fiber` to use for fiber scheduling. The most basic example would be:

```c++
#include <cstdio>
#include <s2fiber.h>

static void fiber_func(s2::fiber &fib)
{
	for (int i = 0; i < 10; i++) {
		printf("Fiber tick %d\n", i);
		fib.yield();
	}
	printf("Finished!\n");
}

int main()
{
	s2::fiber fib(fiber_func);

	while (!fib.isfinished()) {
		printf("Not finished yet..\n");
		fib.resume();
	}

	return 0;
}
```

Note that on Mac OS, you are currently required to build with `-D_XOPEN_SOURCE`.

## `s2test.h`

Provides functions for unit testing. The most basic example would be:

```c++
#include <s2test.h>

int main()
{
	s2::test_begin();

	S2_TEST(true == true);
	S2_TEST(false == false);

	s2::test_end();

	return s2::test_retval();
}
```

Tests can also be grouped by simply calling `s2::test_group(const char* group)` before each group of `S2_TEST()` macros.

# Todo

* Ability to replace standard lib functions such as `malloc`, `printf`, and `fopen` with custom functions
* Consider string using stack memory for short strings

# License

Scratch2 is MIT licensed.
