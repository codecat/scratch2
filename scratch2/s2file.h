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

	class file
	{
	private:
		char* m_filename;

		filemode m_mode;
		void* m_fh;
		size_t m_size;

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

#if defined(S2_USING_STRING)
		s2::string readline();
#else
		char* readline();
#endif
		void writeline(const char* buffer);

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
		fseek((FILE*)m_fh, pos_orig, SEEK_SET);
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
	fseek((FILE*)m_fh, pos, SEEK_SET);
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

#if defined(S2_USING_STRING)
s2::string s2::file::readline()
#else
char* s2::file::readline()
#endif
{
	size_t bufferSize = 128;
	size_t bufferOffset = 0;
	char* buffer = (char*)malloc(bufferSize + 1);

	long startPos = ftell((FILE*)m_fh);

	while (true) {
		size_t bytesRead = fread(buffer + bufferOffset, 1, 128, (FILE*)m_fh);
		buffer[bytesRead] = '\0';

		if (bytesRead == 0) {
			// we hit eof early
			break;
		}

		char* newline = strchr(buffer, '\n');
		if (newline != nullptr) {
			// we found a newline
			*newline = '\0';
			if (newline > buffer && *(newline - 1) == '\r') {
				*(newline - 1) = '\0';
			}
			fseek((FILE*)m_fh, startPos + (newline - buffer) + 1, SEEK_SET);
			break;
		}

		if (bytesRead < 128) {
			// we've hit eof and there's no newline
			break;
		}

		// we need to read more data
		bufferSize += 128;
		bufferOffset += 128;
		buffer = (char*)realloc(buffer, bufferSize + 1);
	}

#if defined(S2_USING_STRING)
	s2::string ret(buffer);
	free(buffer);
	return ret;
#else
	return buffer;
#endif
}


void s2::file::writeline(const char* buffer)
{
	size_t len = strlen(buffer);
	fwrite(buffer, 1, len, (FILE*)m_fh);
	fputc('\n', (FILE*)m_fh);
	m_size += len + 1;
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
