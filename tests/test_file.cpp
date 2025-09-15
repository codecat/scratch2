#include <s2file.h>

#include <s2test.h>

#include "structs.h"

#include <cstring>

void test_file()
{
	s2::test_group("file");

	s2::file not_exist("404.bin");
	try {
		not_exist.open(s2::filemode::read);
		S2_TEST(false /* we shouldn't get here */);
	} catch (s2::fileexception& ex) {
		int errcode = ex.posix_error();
		S2_TEST(errcode == ENOENT);
		const char* err = ex.posix_error_string();
		S2_TEST(!strcmp(err, "No such file or directory"));
	}

	s2::file file("test.bin");

	file.open(s2::filemode::write);
	S2_TEST(file.get_mode() == s2::filemode::write);
	unsigned int num = 0xC0DECA77;
	file.write(&num, sizeof(unsigned int));
	S2_TEST(file.pos() == 4);
	S2_TEST(file.eof());
	file.close();
	S2_TEST(file.get_mode() == s2::filemode::none);

	num = 0;
	S2_TEST(num == 0);

	file.open(s2::filemode::read);
	S2_TEST(file.get_mode() == s2::filemode::read);
	S2_TEST(file.size() == sizeof(unsigned int));
	S2_TEST(!file.eof());
	file.read(&num, sizeof(unsigned int));
	S2_TEST(num == 0xC0DECA77);
	S2_TEST(file.eof());
	file.close();
	S2_TEST(file.get_mode() == s2::filemode::none);

	{
		file.open(s2::filemode::write);
		S2_TEST(file.eof());
		S2_TEST(file.get_mode() == s2::filemode::write);
		S2_TEST(file.size() == 0);
		Bar bar;
		S2_TEST(_numBarInstances == 1);
		bar.a = 1;
		bar.b = 2;
		bar.c = 3;
		bar.d = 4;
		file.write(bar);
		S2_TEST(file.eof());
		S2_TEST(file.pos() == sizeof(Bar));
		S2_TEST(file.size() == sizeof(Bar));
		file.close();
		S2_TEST(file.get_mode() == s2::filemode::none);

		file.open(s2::filemode::read);
		S2_TEST(file.get_mode() == s2::filemode::read);
		S2_TEST(file.size() == sizeof(Bar));
		S2_TEST(!file.eof());
		Bar bar2 = file.read<Bar>();
		S2_TEST(file.eof());
		S2_TEST(_numBarInstances == 2);
		S2_TEST(bar2.a == bar.a);
		S2_TEST(bar2.b == bar.b);
		S2_TEST(bar2.c == bar.c);
		S2_TEST(bar2.d == bar.d);
		file.close();
		S2_TEST(file.get_mode() == s2::filemode::none);
	}

	{
		s2::file textFile("test.txt");
		textFile.open(s2::filemode::write);
		textFile.writeline("Line 1");
		textFile.writeline("Line 2");
		textFile.writeline("Line 3");
	}

	{
		s2::file textFile("test.txt");
		textFile.open(s2::filemode::read);
		S2_TEST(!strcmp(textFile.readline(), "Line 1"));
		S2_TEST(!strcmp(textFile.readline(), "Line 2"));
		S2_TEST(!strcmp(textFile.readline(), "Line 3"));
	}

	S2_TEST(_numBarInstances == 0);
}
