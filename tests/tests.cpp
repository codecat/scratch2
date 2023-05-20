#include <s2test.h>

extern void test_string();
extern void test_list();
extern void test_dict();
extern void test_hashtable();
extern void test_set();
extern void test_file();
extern void test_ref();
extern void test_func();

int main()
{
	s2::test_begin();

	test_string();
	test_list();
	test_dict();
	test_hashtable();
	test_set();
	test_file();
	test_ref();
	test_func();

	s2::test_end();

	return s2::test_retval();
}
