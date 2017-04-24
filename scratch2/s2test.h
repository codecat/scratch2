#pragma once

#define S2_USING_TEST

namespace s2
{
	void test_begin();
	void test_group(const char* group);
	void test_test(bool condition, const char* code, const char* filename, int line);
	void test_end();

	int test_retval();
}

#define S2_TEST(condition) s2::test_test((condition), #condition, __FILE__, __LINE__)

#ifdef S2_IMPL
#include <cstdio>

namespace s2
{
	static const char* _testCurGroup;
	static int _testNumGroups;

	static int _testNumTests;
	static int _testNumPassed;
	static int _testNumFailed;
}

void s2::test_begin()
{
	_testCurGroup = "Tests";
	_testNumGroups = 0;

	_testNumTests = 0;
	_testNumPassed = 0;
	_testNumFailed = 0;
}

void s2::test_group(const char* group)
{
	printf("%s:\n", group);
	_testCurGroup = group;
	_testNumGroups++;
}

void s2::test_test(bool condition, const char* code, const char* filename, int line)
{
	_testNumTests++;
	if (condition) {
		printf("    OK   %s\n", code);
		_testNumPassed++;
	} else {
		printf("  [FAIL] %s (%s : %d)\n", code, filename, line);
		_testNumFailed++;
	}
}

void s2::test_end()
{
	printf("\nTest results in %d groups:\n", _testNumGroups);
	printf("  %d total\n", _testNumTests);
	printf("  %d passed\n", _testNumPassed);
	printf("  %d failed\n", _testNumFailed);
}

int s2::test_retval()
{
	if (_testNumFailed == 0) {
		return 0;
	}
	return 1;
}
#endif
