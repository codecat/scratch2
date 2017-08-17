#pragma once

#define S2_USING_STRING

#include <cstddef>

namespace s2
{
	class stringsplit;

	class string
	{
	friend class stringsplit;

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

		int indexof(const char* sz) const;
		bool contains(const char* sz) const;
		bool startswith(const char* sz) const;
		bool endswith(const char* sz) const;
		stringsplit split(const char* delim, int limit = 0) const;

		string substr(int start) const;
		string substr(int start, int len) const;

		void append(const char* sz);
		void append(const char* sz, size_t len);
		void append(const char* sz, size_t start, size_t len);

		void insert(const char* sz, size_t pos);
		void insert(const char* sz, size_t pos, size_t len);
		void remove(size_t pos, size_t len);

		void setf(const char* format, ...);
		void appendf(const char* format, ...);

		string &operator =(const char* sz);
		string &operator =(const string &str);

		string &operator +=(const char* sz);
		string &operator +=(const string &str);

		string trim() const;
		string trim(const char* sz) const;

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

	class stringsplit
	{
	private:
		char** m_buffer = nullptr;
		size_t m_length = 0;

	public:
		stringsplit(const char* sz, const char* delim, int limit = 0);
		~stringsplit();

		size_t len() const;
		string operator[](size_t index) const;

	private:
		void add(const char* sz, size_t len);
	};
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

int s2::string::indexof(const char* sz) const
{
	char* p = strstr(m_buffer, sz);
	if (p == nullptr) {
		return -1;
	}
	return (int)(p - m_buffer);
}

bool s2::string::contains(const char* sz) const
{
	return strstr(m_buffer, sz) != nullptr;
}

bool s2::string::startswith(const char* sz) const
{
	return strstr(m_buffer, sz) == m_buffer;
}

bool s2::string::endswith(const char* sz) const
{
	return !strcmp(m_buffer + m_length - strlen(sz), sz);
}

s2::stringsplit s2::string::split(const char* delim, int limit) const
{
	return stringsplit(m_buffer, delim, limit);
}

s2::string s2::string::substr(int start) const
{
	if (m_length == 0) {
		return "";
	}
	while (start < 0) {
		start += m_length;
	}
	if ((size_t)start >= m_length) {
		return "";
	}
	return string(m_buffer + start);
}

s2::string s2::string::substr(int start, int len) const
{
	if (m_length == 0) {
		return "";
	}
	while (start < 0) {
		start += m_length;
	}
	if ((size_t)start >= m_length) {
		return "";
	}
	int remainder = strlen(m_buffer) + start;
	if (len > remainder) {
		len = remainder;
	}
	return string(m_buffer + start, len);
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

void s2::string::insert(const char* sz, size_t pos)
{
	insert(sz, pos, strlen(sz));
}

void s2::string::insert(const char* sz, size_t pos, size_t len)
{
	ensure_memory(m_length + len + 1);
	memmove(m_buffer + pos + len, m_buffer + pos, m_length - pos);
	memcpy(m_buffer + pos, sz, len);
	m_length += len;
	m_buffer[m_length] = '\0';
}

void s2::string::remove(size_t pos, size_t len)
{
	memmove(m_buffer + pos, m_buffer + pos + len, m_length - pos - len);
	m_length -= len;
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

s2::string s2::string::trim() const
{
	return trim("\n\r\t ");
}

s2::string s2::string::trim(const char* sz) const
{
	int num = strlen(sz);
	if (num == 0) {
		return *this;
	}

	int len = strlen(m_buffer);
	char* p = m_buffer;
	while (*p != '\0') {
		if (strchr(sz, *p) == nullptr) {
			break;
		}
		p++;
	}

	char* pp = m_buffer + (len - 1);
	while (pp > p) {
		if (strchr(sz, *pp) == nullptr) {
			break;
		}
		pp--;
	}

	return string(p, (pp - p) + 1);
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

	s2::string ret = buffer;
	free(buffer);
	return ret;
}

s2::stringsplit::stringsplit(const char* sz, const char* delim, int limit)
{
	const char* p = sz;
	size_t len = strlen(sz);
	size_t lenDelim = strlen(delim);

	while (*p != '\0') {
		const char* pos = strstr(p, delim);

		if (pos == nullptr || (limit > 0 && m_length + 1 == limit)) {
			add(p, len - (p - sz));
			return;
		}

		add(p, pos - p);
		p = pos + lenDelim;
	}
}

s2::stringsplit::~stringsplit()
{
	for (size_t i = 0; i < m_length; i++) {
		free(m_buffer[i]);
	}
	free(m_buffer);
}

size_t s2::stringsplit::len() const
{
	return m_length;
}

s2::string s2::stringsplit::operator[](size_t index) const
{
	return m_buffer[index];
}

void s2::stringsplit::add(const char* sz, size_t len)
{
	m_length++;
	m_buffer = (char**)realloc(m_buffer, m_length * sizeof(char*));
	char* p = (char*)malloc(len + 1);
	memcpy(p, sz, len);
	p[len] = '\0';
	m_buffer[m_length - 1] = p;
}
#endif
