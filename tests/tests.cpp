#include <s2test.h>

extern void test_string();
extern void test_list();
extern void test_dict();
extern void test_file();
extern void test_ref();

int main()
{
	s2::test_begin();

	test_string();
	test_list();
	test_dict();
	test_file();
	test_ref();

	s2::test_end();

	return s2::test_retval();
}
