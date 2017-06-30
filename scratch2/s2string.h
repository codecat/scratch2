#pragma once

#define S2_USING_STRING

#include <cstddef>

namespace s2
{
	class string
	{
	private:
		char* m_buffer;
		size_t m_length;
		size_t m_allocSize;

	public:
		string();
		string(const char* sz);
		string(const char* sz, size_t len);
		string(const char* sz, size_t start, size_t len);
		string(const string &str);
		~string();

		size_t len() const;
		const char* c_str() const;

		void append(const char* sz);
		void append(const char* sz, size_t len);
		void append(const char* sz, size_t start, size_t len);

		void setf(const char* format, ...);
		void appendf(const char* format, ...);

		string &operator =(const char* sz);
		string &operator =(const string &str);

		string &operator +=(const char* sz);
		string &operator +=(const string &str);

		bool operator ==(const char* sz) const;
		bool operator ==(const string &str) const;

		bool operator !=(const char* sz) const;
		bool operator !=(const string &str) const;

		operator const char*() const;

		char &operator [](int index);
		const char &operator [](int index) const;

	private:
		void ensure_memory(size_t size);
		void resize_memory(size_t size);
	};

	string strprintf(const char* format, ...);
}

#ifdef S2_IMPL
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <cassert>
#include <cstdarg>
#include <cstdio>

const size_t min_buffer_size = 24;

s2::string::string()
{
	m_length = 0;
	m_buffer = nullptr;
	resize_memory(1);
	m_buffer[0] = '\0';
}

s2::string::string(const char* sz)
	: string(sz, 0, strlen(sz))
{
}

s2::string::string(const char* sz, size_t len)
	: string(sz, 0, len)
{
}

s2::string::string(const char* sz, size_t start, size_t len)
{
	m_length = len;
	m_buffer = nullptr;
	resize_memory(len + 1);
	memcpy(m_buffer + start, sz, len);
	m_buffer[len] = '\0';
}

s2::string::string(const s2::string &str)
	: string(str.m_buffer)
{
}

s2::string::~string()
{
	free(m_buffer);
}

size_t s2::string::len() const
{
	return m_length;
}

const char* s2::string::c_str() const
{
	return m_buffer;
}

void s2::string::append(const char* sz)
{
	append(sz, 0, strlen(sz));
}

void s2::string::append(const char* sz, size_t len)
{
	append(sz, 0, len);
}

void s2::string::append(const char* sz, size_t start, size_t len)
{
	ensure_memory(m_length + len + 1);
	memcpy(m_buffer + m_length, sz + start, len);
	m_length += len;
	m_buffer[m_length] = '\0';
}

void s2::string::setf(const char* format, ...)
{
	ensure_memory(min_buffer_size);

	va_list vl;
	va_start(vl, format);
	int len = vsnprintf(m_buffer, m_allocSize, format, vl);
	va_end(vl);

	if (len >= min_buffer_size) {
		ensure_memory(len + 1);

		va_list vl;
		va_start(vl, format);
		vsnprintf(m_buffer, m_allocSize, format, vl);
		va_end(vl);
	}

	m_length = len;
}

void s2::string::appendf(const char* format, ...)
{
	char* buffer = (char*)malloc(min_buffer_size);

	va_list vl;
	va_start(vl, format);
	int len = vsnprintf(buffer, min_buffer_size, format, vl);
	va_end(vl);

	if (len >= min_buffer_size) {
		buffer = (char*)realloc(buffer, len + 1);

		va_list vl;
		va_start(vl, format);
		len = vsnprintf(buffer, len + 1, format, vl);
		va_end(vl);
	}

	append(buffer, 0, len);

	free(buffer);
}

s2::string &s2::string::operator =(const char* sz)
{
	m_length = strlen(sz);
	ensure_memory(m_length + 1);
	memcpy(m_buffer, sz, m_length);
	m_buffer[m_length] = '\0';
	return *this;
}

s2::string &s2::string::operator =(const s2::string &str)
{
	return operator =(str.m_buffer);
}

s2::string &s2::string::operator +=(const char* sz)
{
	append(sz);
	return *this;
}

s2::string &s2::string::operator +=(const s2::string &str)
{
	return operator +=(str.m_buffer);
}

bool s2::string::operator ==(const char* sz) const
{
	return !strcmp(m_buffer, sz);
}

bool s2::string::operator ==(const s2::string &str) const
{
	return !strcmp(m_buffer, str.m_buffer);
}

bool s2::string::operator !=(const char* sz) const
{
	return !!strcmp(m_buffer, sz);
}

bool s2::string::operator !=(const string &str) const
{
	return !!strcmp(m_buffer, str.m_buffer);
}

s2::string::operator const char*() const
{
	return m_buffer;
}

char &s2::string::operator [](int index)
{
	return m_buffer[index];
}

const char &s2::string::operator [](int index) const
{
	return m_buffer[index];
}

void s2::string::ensure_memory(size_t size)
{
	if (m_allocSize >= size) {
		return;
	}
	resize_memory(size);
}

void s2::string::resize_memory(size_t size)
{
	m_allocSize = size;
	m_buffer = (char*)realloc(m_buffer, m_allocSize);
}

s2::string s2::strprintf(const char* format, ...)
{
	char* buffer = (char*)malloc(min_buffer_size);

	va_list vl;
	va_start(vl, format);
	int len = vsnprintf(buffer, min_buffer_size, format, vl);
	va_end(vl);

	if (len >= min_buffer_size) {
		buffer = (char*)realloc(buffer, len + 1);

		va_list vl;
		va_start(vl, format);
		len = vsnprintf(buffer, len + 1, format, vl);
		va_end(vl);
	}

	return s2::string(buffer);
}
#endif
