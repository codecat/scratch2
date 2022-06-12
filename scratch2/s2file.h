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

		filemode m_mode = filemode::none;
		void* m_fh;
		size_t m_size;

		newlinemode m_newlines = newlinemode::lf;

		char* m_readData = nullptr;

	private:
		file(const file &copy);

	public:
		file();
		file(const char* filename);
		~file();

		void open(const char* filename, filemode mode);
		void open(filemode mode);
		void close();

		size_t size();
		bool eof();
		void flush();

		filemode get_mode();

		void set_pos(size_t pos);
		size_t pos();

		size_t read(void* buffer, size_t size);
		size_t write(const void* buffer, size_t size);

		void set_newline(newlinemode mode);

		const char* readline();
		const char* readtoend(size_t* out_size = nullptr);

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
		void write(const T &o)
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

#if defined(_MSC_VER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

class ToWide
{
private:
	wchar_t* m_buffer;

public:
	ToWide(const char* src)
	{
		int size = MultiByteToWideChar(CP_UTF8, 0, src, -1, 0, 0);
		m_buffer = (wchar_t*)malloc(size * sizeof(wchar_t));
		MultiByteToWideChar(CP_UTF8, 0, src, -1, m_buffer, size);
	}
	~ToWide() { free(m_buffer); }
	operator const wchar_t* () { return m_buffer; }
	wchar_t* buffer() { return m_buffer; }
};

class ToUtf8
{
private:
	char* m_buffer;

public:
	ToUtf8(const wchar_t* src)
	{
		int size = WideCharToMultiByte(CP_UTF8, 0, src, -1, 0, 0, 0, 0);
		m_buffer = (char*)malloc(size * sizeof(char));
		WideCharToMultiByte(CP_UTF8, 0, src, -1, m_buffer, size, 0, 0);
	}
	~ToUtf8() { free(m_buffer); }
	operator const char* () { return m_buffer; }
	char* buffer() { return m_buffer; }
};
#endif

s2::file::file()
{
	m_filename = nullptr;

	m_fh = nullptr;
	m_size = 0;
}

s2::file::file(const char* filename)
	: file()
{
	m_filename = (char*)malloc(strlen(filename) + 1);
	strcpy(m_filename, filename);
}

s2::file::~file()
{
	if (m_filename != nullptr) {
		free(m_filename);
	}
	if (m_fh != nullptr) {
		fclose((FILE*)m_fh);
	}

	if (m_readData != nullptr) {
		free(m_readData);
	}
}

void s2::file::open(const char* filename, s2::filemode mode)
{
	if (m_filename != nullptr) {
		free(m_filename);
	}

	m_filename = (char*)malloc(strlen(filename) + 1);
	strcpy(m_filename, filename);

	open(mode);
}

void s2::file::open(s2::filemode mode)
{
	if (m_filename == nullptr) {
		return;
	}

#if defined(_MSC_VER)
	const wchar_t* openmode;
	switch (mode) {
	case s2::filemode::read: openmode = L"rb"; break;
	case s2::filemode::write: openmode = L"wb"; break;
	case s2::filemode::append: openmode = L"ab"; break;
	default: return;
	}
#else
	const char* openmode;
	switch (mode) {
	case s2::filemode::read: openmode = "rb"; break;
	case s2::filemode::write: openmode = "wb"; break;
	case s2::filemode::append: openmode = "ab"; break;
	default: return;
	}
#endif

#if defined(_MSC_VER)
	m_fh = _wfopen(ToWide(m_filename), openmode);
#else
	m_fh = fopen(m_filename, openmode);
#endif
	if (m_fh == nullptr) {
		m_mode = s2::filemode::none;
		return;
	}

	m_mode = mode;
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

void s2::file::flush()
{
	if (m_fh == nullptr) {
		return;
	}
	fflush((FILE*)m_fh);
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

size_t s2::file::write(const void* buffer, size_t size)
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
		return "";
	}

	if (m_readData == nullptr) {
		m_readData = (char*)malloc(S2_FILE_READLINE_BUFFERSIZE + 1);
	}

	if (fgets(m_readData, S2_FILE_READLINE_BUFFERSIZE, (FILE*)m_fh) == nullptr) {
		return "";
	}

	size_t len = strlen(m_readData);
	if (len > 0) {
		char &lastChar = m_readData[len - 1];
		if (lastChar == '\n' || lastChar == '\r') {
			lastChar = '\0';
		}
	}
	if (len > 1) {
		char &almostLastChar = m_readData[len - 2];
		if (almostLastChar == '\n' || almostLastChar == '\r') {
			almostLastChar = '\0';
		}
	}

	return m_readData;
}

const char* s2::file::readtoend(size_t* out_size)
{
	if (m_fh == nullptr) {
		return nullptr;
	}

	if (m_readData != nullptr) {
		free(m_readData);
	}

	size_t readsize = size() - pos();
	m_readData = (char*)malloc(readsize + 1);
	m_readData[readsize] = '\0';

	read(m_readData, readsize);

	if (out_size != nullptr) {
		*out_size = readsize;
	}

	return m_readData;
}

void s2::file::writeline()
{
	if (m_fh == nullptr) {
		return;
	}

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
	if (m_fh == nullptr) {
		return;
	}

	size_t len = strlen(str);
	if (len > 0) {
		fwrite(str, len, 1, (FILE*)m_fh);
	}
	writeline();
}

bool s2::file_exists(const char* filename)
{
#if defined(_MSC_VER)
	FILE* fh = _wfopen(ToWide(filename), L"rb");
#else
	FILE* fh = fopen(filename, "rb");
#endif
	if (fh == nullptr) {
		return false;
	}
	fclose(fh);
	return true;
}

size_t s2::file_size(const char* filename)
{
#if defined(_MSC_VER)
	FILE* fh = _wfopen(ToWide(filename), L"rb");
#else
	FILE* fh = fopen(filename, "rb");
#endif
	if (fh == nullptr) {
		return 0;
	}
	fseek(fh, 0, SEEK_END);
	size_t ret = ftell(fh);
	fclose(fh);
	return ret;
}
#endif
