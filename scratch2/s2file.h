#pragma once

#define S2_USING_FILE

#include <cstddef>

namespace s2
{
	enum class filemode
	{
		none,
		read,
		write,
		append,
	};

	enum class newlinemode
	{
		cr, // \r
		lf, // \n
		crlf, // \r\n
	};

	class file
	{
	private:
		char* m_filename;

		filemode m_mode;
		void* m_fh;
		size_t m_size;

		newlinemode m_newlines = newlinemode::lf;

		char* m_lineRead = nullptr;

	private:
		file(const file &copy);

	public:
		file(const char* filename);
		~file();

		void open(filemode mode);
		void close();

		size_t size();
		bool eof();

		filemode get_mode();

		void set_pos(size_t pos);
		size_t pos();

		size_t read(void* buffer, size_t size);
		size_t write(void* buffer, size_t size);

		void set_newline(newlinemode mode);

		const char* readline();

		void writeline();
		void writeline(const char* str);

		template<typename T>
		void read(T &o)
		{
			read(&o, sizeof(T));
		}

		template<typename T>
		T read()
		{
			T ret;
			read(&ret, sizeof(T));
			return ret;
		}

		template<typename T>
		void write(T &o)
		{
			write(&o, sizeof(T));
		}
	};

	bool file_exists(const char* filename);
	size_t file_size(const char* filename);
}

#ifdef S2_IMPL

#ifndef S2_FILE_READLINE_BUFFERSIZE
#define S2_FILE_READLINE_BUFFERSIZE 1024
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

s2::file::file(const char* filename)
{
	m_filename = (char*)malloc(strlen(filename) + 1);
	strcpy(m_filename, filename);

	m_fh = nullptr;
	m_size = 0;
}

s2::file::~file()
{
	free(m_filename);
	if (m_fh != nullptr) {
		fclose((FILE*)m_fh);
	}

	if (m_lineRead != nullptr) {
		free(m_lineRead);
	}
}

void s2::file::open(s2::filemode mode)
{
	const char* openmode = "";
	if (mode == s2::filemode::read) {
		openmode = "rb";
	} else if (mode == s2::filemode::write) {
		openmode = "wb";
	} else if (mode == s2::filemode::append) {
		openmode = "ab";
	} else {
		return;
	}

	m_mode = mode;
	m_fh = fopen(m_filename, openmode);
	m_size = size();
}

void s2::file::close()
{
	if (m_fh == nullptr) {
		return;
	}
	fclose((FILE*)m_fh);
	m_mode = s2::filemode::none;
	m_fh = nullptr;
	m_size = 0;
}

size_t s2::file::size()
{
	if (m_fh == nullptr) {
		return 0;
	}

	if (m_size == 0) {
		size_t pos_orig = ftell((FILE*)m_fh);
		fseek((FILE*)m_fh, 0, SEEK_END);
		m_size = ftell((FILE*)m_fh);
		fseek((FILE*)m_fh, (long)pos_orig, SEEK_SET);
	}

	return m_size;
}

bool s2::file::eof()
{
	if (m_fh == nullptr) {
		return true;
	}
	size_t pos = ftell((FILE*)m_fh);
	return pos >= m_size;
}

s2::filemode s2::file::get_mode()
{
	return m_mode;
}

void s2::file::set_pos(size_t pos)
{
	if (m_fh == nullptr) {
		return;
	}
	fseek((FILE*)m_fh, (long)pos, SEEK_SET);
}

size_t s2::file::pos()
{
	if (m_fh == nullptr) {
		return 0;
	}
	return ftell((FILE*)m_fh);
}

size_t s2::file::read(void* buffer, size_t size)
{
	if (m_fh == nullptr) {
		return 0;
	}
	return fread(buffer, 1, size, (FILE*)m_fh);
}

size_t s2::file::write(void* buffer, size_t size)
{
	if (m_fh == nullptr) {
		return 0;
	}
	m_size += size;
	return fwrite(buffer, 1, size, (FILE*)m_fh);
}

void s2::file::set_newline(newlinemode mode)
{
	m_newlines = mode;
}

const char* s2::file::readline()
{
	if (m_fh == nullptr) {
		return nullptr;
	}

	if (m_lineRead == nullptr) {
		m_lineRead = (char*)malloc(S2_FILE_READLINE_BUFFERSIZE + 1);
	}

	if (fgets(m_lineRead, S2_FILE_READLINE_BUFFERSIZE, (FILE*)m_fh) == nullptr) {
		return nullptr;
	}

	size_t len = strlen(m_lineRead);
	if (len > 0) {
		char &lastChar = m_lineRead[len - 1];
		if (lastChar == '\n' || lastChar == '\r') {
			lastChar = '\0';
		}
	}
	if (len > 1) {
		char &almostLastChar = m_lineRead[len - 2];
		if (almostLastChar == '\n' || almostLastChar == '\r') {
			almostLastChar = '\0';
		}
	}

	return m_lineRead;
}

void s2::file::writeline()
{
	const char* newline = "\r\n";
	if (m_newlines == newlinemode::cr) {
		newline = "\r";
	} else if (m_newlines == newlinemode::lf) {
		newline = "\n";
	}
	fwrite(newline, strlen(newline), 1, (FILE*)m_fh);
}

void s2::file::writeline(const char* str)
{
	fwrite(str, strlen(str), 1, (FILE*)m_fh);
	writeline();
}

bool s2::file_exists(const char* filename)
{
	FILE* fh = fopen(filename, "rb");
	if (fh == nullptr) {
		return false;
	}
	fclose(fh);
	return true;
}

size_t s2::file_size(const char* filename)
{
	FILE* fh = fopen(filename, "rb");
	if (fh == nullptr) {
		return 0;
	}
	fseek(fh, 0, SEEK_END);
	size_t ret = ftell(fh);
	fclose(fh);
	return ret;
}
#endif
