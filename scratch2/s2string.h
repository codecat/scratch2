#pragma once

#define S2_USING_STRING

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>

namespace s2
{
	class stringsplit;
	class stringview;

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
		size_t allocsize() const;
		const char* c_str() const;
		bool is_null() const;

		int indexof(char c) const;
		int indexof(const char* sz) const;
		int lastindexof(char c) const;
		int lastindexof(const char* sz) const;

		bool contains(const char* sz) const;
		bool contains_nocase(const char* sz) const;
		bool startswith(const char* sz) const;
		bool endswith(const char* sz) const;
		stringsplit split(const char* delim, int limit = 0) const;
		stringsplit commandlinesplit() const;

		string substr(intptr_t start) const;
		string substr(intptr_t start, intptr_t len) const;

		void append(char c);

		void append(const char* sz);
		void append(const char* sz, size_t len);
		void append(const char* sz, size_t start, size_t len);

		void insert(const char* sz, size_t pos);
		void insert(const char* sz, size_t pos, size_t len);
		void remove(size_t pos, size_t len);

		string replace(char find, char replace) const;
		string replace(const char* find, const char* replace) const;
		string replace(const char* findchars, char replace) const;

		void setf(const char* format, ...);
		void appendf(const char* format, ...);

		string &operator =(const char* sz);
		string &operator =(const string &str);

		string &operator +=(const char* sz);
		string &operator +=(const string &str);

		string operator +(const char* sz) const;
		string operator +(const string &str) const;

		string trim() const;
		string trim(const char* sz) const;

		string tolower() const;
		string toupper() const;

		int as_int() const;
		int64_t as_large_int() const;
		uint32_t as_uint() const;
		uint64_t as_large_uint() const;
		float as_float() const;
		double as_double() const;
		bool as_bool() const;

		bool operator ==(const char* sz) const;
		bool operator ==(const string &str) const;

		bool operator !=(const char* sz) const;
		bool operator !=(const string &str) const;

		operator const char*() const;

		char &operator [](int index);
		const char &operator [](int index) const;

		void ensure_memory(size_t size);

	private:
		void resize_memory(size_t size);
	};

	bool operator ==(const char* sz, const string &str);

	string operator +(const char* lhs, const string &rhs);

	string strprintf(const char* format, ...);

	class stringsplit
	{
	private:
		char** m_buffer = nullptr;
		size_t m_length = 0;

	public:
		stringsplit(const char* sz, const char* delim, int limit = 0);
		stringsplit(const char* sz, bool commandLine);
		stringsplit(const stringsplit &copy);
		~stringsplit();

		size_t len() const;
		string operator[](size_t index) const;

		const char* c_str(size_t index) const;

	private:
		void add(const char* sz, size_t len);
	};

	class stringview
	{
	private:
		const char* m_str;
		size_t m_len;

	public:
		inline stringview()
		{
			m_str = "";
			m_len = 0;
		}

		inline stringview(const char* str)
		{
			m_str = str;
			m_len = strlen(str);
		}

		inline stringview(const char* str, size_t len)
		{
			m_str = str;
			m_len = len;
		}

		inline stringview(const string& str)
		{
			m_str = str.c_str();
			m_len = str.len();
		}

		inline stringview(const stringview& other)
		{
			m_str = other.m_str;
			m_len = other.m_len;
		}

		inline void operator=(const stringview& other)
		{
			m_str = other.m_str;
			m_len = other.m_len;
		}

		inline operator const char* () const { return m_str; }
		inline const char* c_str() const { return m_str; }
		inline size_t len() const { return m_len; }

		inline bool operator==(const char* str) const { return !strcmp(m_str, str); }
		inline bool operator!=(const char* str) const { return !!strcmp(m_str, str); }
	};

#if defined(_MSC_VER)
	class str_to_wide
	{
	private:
		wchar_t* m_buffer;
		int m_size;

	public:
		str_to_wide(const char* src);
		inline ~str_to_wide() { free(m_buffer); }
		inline operator const wchar_t* () { return m_buffer; }
		inline wchar_t* buffer() { return m_buffer; }
		inline int size() { return m_size; }
	};

	class str_to_utf8
	{
	private:
		char* m_buffer;
		int m_size;

	public:
		str_to_utf8(const wchar_t* src);
		inline ~str_to_utf8() { free(m_buffer); }
		inline operator const char* () { return m_buffer; }
		inline char* buffer() { return m_buffer; }
		inline int size() { return m_size; }
		inline operator s2::string() { return s2::string(m_buffer); }
	};
#endif
}

#ifdef S2_IMPL
#include <cstdlib>
#include <cstdint>
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <cctype>

#if defined(_MSC_VER)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

const size_t min_buffer_size = 24;

s2::string::string()
{
	m_length = 0;
	m_buffer = nullptr;
	m_allocSize = 0;
}

s2::string::string(const char* sz)
	: string(sz, 0, sz != nullptr ? strlen(sz) : 0)
{
}

s2::string::string(const char* sz, size_t len)
	: string(sz, 0, len)
{
}

s2::string::string(const char* sz, size_t start, size_t len)
	: string()
{
	if (sz == nullptr) {
		return;
	}
	m_length = len;
	resize_memory(len + 1);
	memcpy(m_buffer + start, sz, len);
	m_buffer[len] = '\0';
}

s2::string::string(const s2::string &str)
	: string(str.m_buffer, 0, str.m_length)
{
}

s2::string::~string()
{
	if (m_buffer != nullptr) {
		free(m_buffer);
	}
}

size_t s2::string::len() const
{
	return m_length;
}

size_t s2::string::allocsize() const
{
	return m_allocSize;
}

const char* s2::string::c_str() const
{
	if (m_buffer == nullptr) {
		return "";
	}
	return m_buffer;
}

bool s2::string::is_null() const
{
	return m_buffer == nullptr;
}

int s2::string::indexof(char c) const
{
	if (m_buffer == nullptr) {
		return -1;
	}

	char* p = m_buffer;
	while (*p != '\0') {
		if (*p == c) {
			return (int)(p - m_buffer);
		}
		p++;
	}
	return -1;
}

int s2::string::indexof(const char* sz) const
{
	if (m_buffer == nullptr) {
		return -1;
	}

	char* p = strstr(m_buffer, sz);
	if (p == nullptr) {
		return -1;
	}
	return (int)(p - m_buffer);
}

int s2::string::lastindexof(char c) const
{
	if (m_buffer == nullptr) {
		return -1;
	}

	char* p = m_buffer + m_length;
	while (p != m_buffer) {
		p--;
		if (*p == c) {
			return (int)(p - m_buffer);
		}
	}
	return -1;
}

int s2::string::lastindexof(const char* sz) const
{
	if (m_buffer == nullptr) {
		return -1;
	}

	char* lp = strstr(m_buffer, sz);
	char* p = lp;
	while (p != nullptr) {
		p = strstr(p + 1, sz);
		if (p != nullptr) {
			lp = p;
		}
	}
	if (lp == nullptr) {
		return -1;
	}
	return (int)(lp - m_buffer);
}

bool s2::string::contains(const char* sz) const
{
	if (m_buffer == nullptr) {
		return false;
	}
	return strstr(m_buffer, sz) != nullptr;
}

bool s2::string::contains_nocase(const char* sz) const
{
	if (m_buffer == nullptr) {
		return false;
	}

	size_t matched = 0;
	for (size_t i = 0; i < m_length; i++) {
		char c = ::tolower(m_buffer[i]);
		char ec = ::tolower(sz[matched]);
		if (ec == '\0') {
			return true;
		} else if (c == ec) {
			if (sz[matched + 1] == '\0') {
				return true;
			}
			matched++;
		} else {
			matched = 0;
		}
	}

	return false;
}

bool s2::string::startswith(const char* sz) const
{
	if (m_buffer == nullptr) {
		return false;
	}
	return strstr(m_buffer, sz) == m_buffer;
}

bool s2::string::endswith(const char* sz) const
{
	if (m_buffer == nullptr) {
		return false;
	}
	return !strcmp(m_buffer + m_length - strlen(sz), sz);
}

s2::stringsplit s2::string::split(const char* delim, int limit) const
{
	return stringsplit(m_buffer, delim, limit);
}

s2::stringsplit s2::string::commandlinesplit() const
{
	return stringsplit(m_buffer, true);
}

s2::string s2::string::substr(intptr_t start) const
{
	if (m_length == 0 || m_buffer == nullptr) {
		return "";
	}
	while (start < 0) {
		start += (intptr_t)m_length;
	}
	if ((size_t)start >= m_length) {
		return "";
	}
	return string(m_buffer + start);
}

s2::string s2::string::substr(intptr_t start, intptr_t len) const
{
	if (m_length == 0 || m_buffer == nullptr) {
		return "";
	}
	while (start < 0) {
		start += (intptr_t)m_length;
	}
	if ((size_t)start >= m_length) {
		return "";
	}
	intptr_t remainder = (intptr_t)strlen(m_buffer) + start;
	if (len > remainder) {
		len = remainder;
	}
	return string(m_buffer + start, len);
}

void s2::string::append(char c)
{
	append(&c, 0, 1);
}

void s2::string::append(const char* sz)
{
	if (sz == nullptr) {
		return;
	}
	append(sz, 0, strlen(sz));
}

void s2::string::append(const char* sz, size_t len)
{
	if (sz == nullptr) {
		return;
	}
	append(sz, 0, len);
}

void s2::string::append(const char* sz, size_t start, size_t len)
{
	if (sz == nullptr) {
		return;
	}
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
	if (m_buffer == nullptr) {
		return;
	}
	memmove(m_buffer + pos, m_buffer + pos + len, m_length - pos - len);
	m_length -= len;
	m_buffer[m_length] = '\0';
}

s2::string s2::string::replace(char find, char replace) const
{
	if (m_buffer == nullptr) {
		return *this;
	}

	s2::string ret(*this);
	char* p = ret.m_buffer;
	while (*p != '\0') {
		if (*p == find) {
			*p = replace;
		}
		p++;
	}
	return ret;
}

s2::string s2::string::replace(const char* find, const char* replace) const
{
	if (m_buffer == nullptr || find == nullptr || replace == nullptr) {
		return *this;
	}

	if (*find == '\0') {
		return *this;
	}

	size_t findlen = strlen(find);
	size_t replacelen = strlen(replace);

	s2::string ret(*this);
	int index = 0;
	while (true) {
		char* p = strstr(ret.m_buffer + index, find);
		if (p == nullptr) {
			break;
		}
		index = (int)(p - ret.m_buffer);

		ret.remove(index, findlen);
		ret.insert(replace, index, replacelen);

		index += (int)replacelen;
	}
	return ret;
}

s2::string s2::string::replace(const char* findchars, char replace) const
{
	if (m_buffer == nullptr || findchars == nullptr) {
		return *this;
	}

	if (*findchars == '\0') {
		return *this;
	}

	s2::string ret(*this);
	char* p = ret.m_buffer;
	while (*p != '\0') {
		if (strchr(findchars, *p) != nullptr) {
			*p = replace;
		}
		p++;
	}
	return ret;
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
	if (sz == nullptr) {
		if (m_buffer != nullptr) {
			m_buffer[0] = '\0';
		}
		m_length = 0;
	} else {
		m_length = strlen(sz);
		if (m_length == 0) {
			if (m_buffer != nullptr) {
				m_buffer[0] = '\0';
			}
		} else {
			ensure_memory(m_length + 1);
			memcpy(m_buffer, sz, m_length);
			m_buffer[m_length] = '\0';
		}
	}
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

s2::string s2::string::operator +(const char* sz) const
{
	return string(*this) += sz;
}

s2::string s2::string::operator +(const string &str) const
{
	return string(*this) += str;
}

s2::string s2::string::trim() const
{
	return trim("\n\r\t ");
}

s2::string s2::string::trim(const char* sz) const
{
	if (m_buffer == nullptr || sz == nullptr || strlen(sz) == 0) {
		return *this;
	}

	int len = (int)strlen(m_buffer);
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

s2::string s2::string::tolower() const
{
	if (m_buffer == nullptr) {
		return *this;
	}

	s2::string ret = *this;
	char* p = ret.m_buffer;
	while (*p != '\0') {
		*p = ::tolower(*p);
		p++;
	}
	return ret;
}

s2::string s2::string::toupper() const
{
	if (m_buffer == nullptr) {
		return *this;
	}

	s2::string ret = *this;
	char* p = ret.m_buffer;
	while (*p != '\0') {
		*p = ::toupper(*p);
		p++;
	}
	return ret;
}

int s2::string::as_int() const
{
	if (m_buffer == nullptr) {
		return 0;
	}
	return atoi(m_buffer);
}

int64_t s2::string::as_large_int() const
{
	if (m_buffer == nullptr) {
		return 0;
	}
	return (int64_t)atoll(m_buffer);
}

uint32_t s2::string::as_uint() const
{
	if (m_buffer == nullptr) {
		return 0;
	}
	return (uint32_t)strtoul(m_buffer, nullptr, 10);
}

uint64_t s2::string::as_large_uint() const
{
	if (m_buffer == nullptr) {
		return 0;
	}
	return (uint64_t)strtoull(m_buffer, nullptr, 10);
}

float s2::string::as_float() const
{
	if (m_buffer == nullptr) {
		return 0;
	}
	return (float)atof(m_buffer);
}

double s2::string::as_double() const
{
	if (m_buffer == nullptr) {
		return 0;
	}
	return atof(m_buffer);
}

bool s2::string::as_bool() const
{
	if (m_buffer == nullptr) {
		return false;
	}
	char c = m_buffer[0];
	return c == 't' || c == 'T' || c == 'y' || c == 'Y' || c == '1';
}

bool s2::string::operator ==(const char* sz) const
{
	if (m_buffer == sz) {
		return true;
	}
	if (m_buffer == nullptr) {
		return sz == nullptr || strlen(sz) == 0;
	}
	if (m_buffer != nullptr && sz == nullptr) {
		return false;
	}
	return !strcmp(m_buffer, sz);
}

bool s2::string::operator ==(const s2::string &str) const
{
	return (*this == str.m_buffer);
}

bool s2::string::operator !=(const char* sz) const
{
	return !(*this == sz);
}

bool s2::string::operator !=(const string &str) const
{
	return !(*this == str);
}

s2::string::operator const char*() const
{
	if (m_buffer == nullptr) {
		return "";
	}
	return m_buffer;
}

static char __nullchar = '\0';

char &s2::string::operator [](int index)
{
	if (m_buffer == nullptr) {
		return __nullchar;
	}
	return m_buffer[index];
}

const char &s2::string::operator [](int index) const
{
	if (m_buffer == nullptr) {
		return __nullchar;
	}
	return m_buffer[index];
}

void s2::string::ensure_memory(size_t size)
{
	if (m_buffer != nullptr && m_allocSize >= size) {
		return;
	}

	// Resize in chunks to avoid re-allocating too often
	size_t resize = m_allocSize + m_allocSize / 2;
	if (resize < SIZE_MAX && resize > size) {
		size = resize;
	}

	resize_memory(size);
}

void s2::string::resize_memory(size_t size)
{
	m_allocSize = size;
	m_buffer = (char*)realloc(m_buffer, m_allocSize);
}

bool s2::operator ==(const char* sz, const string &str)
{
	return str == sz;
}

s2::string s2::operator +(const char* lhs, const string &rhs)
{
	return string(lhs) += rhs;
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
	if (sz == nullptr) {
		return;
	}

	// If the input string is empty, we will just have 1 empty string
	if (*sz == 0) {
		add("", 0);
		return;
	}

	const char* p = sz;
	size_t len = strlen(sz);
	size_t lenDelim = strlen(delim);

	// Don't allow empty delimiters
	if (lenDelim == 0) {
		return;
	}

	while (*p != '\0') {
		const char* pos = strstr(p, delim);

		if (pos == nullptr || (limit > 0 && m_length + 1 == limit)) {
			add(p, len - (p - sz));
			return;
		}

		add(p, pos - p);
		p = pos + lenDelim;

		if (*p == '\0') {
			add(p, 0);
		}
	}
}

s2::stringsplit::stringsplit(const char* sz, bool commandLine)
{
	if (sz == nullptr || *sz == '\0') {
		return;
	}

	const char* p = sz;
	s2::string buffer;
	buffer.ensure_memory(128);
	bool inString = false;

	char c;
	do {
		c = *p;
		char cn = *(p + 1);

		if (c == '\\') {
			buffer.append(cn);
			p++;
			continue;
		}

		if (inString) {
			if (c == '"' && (cn == ' ' || cn == '\0')) {
				inString = false;
				continue;
			}

		} else {
			if (c == '"' && buffer.len() == 0) {
				inString = true;
				continue;
			}

			if (c == ' ' && buffer.len() != 0) {
				add(buffer, buffer.len());
				buffer = "";
				continue;
			}
		}

		buffer.append(c);
	} while (*(++p) != '\0');

	if (buffer.len() > 0) {
		add(buffer, buffer.len());
	}
}

s2::stringsplit::stringsplit(const stringsplit &copy)
{
	for (size_t i = 0; i < copy.m_length; i++) {
		const char* s = copy.m_buffer[i];
		add(s, strlen(s));
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

const char* s2::stringsplit::c_str(size_t index) const
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

#if defined(_MSC_VER)
s2::str_to_wide::str_to_wide(const char* src)
{
	m_size = MultiByteToWideChar(CP_UTF8, 0, src, -1, 0, 0);
	m_buffer = (wchar_t*)malloc(m_size * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, src, -1, m_buffer, m_size);
}

s2::str_to_utf8::str_to_utf8(const wchar_t* src)
{
	m_size = WideCharToMultiByte(CP_UTF8, 0, src, -1, 0, 0, 0, 0);
	m_buffer = (char*)malloc(m_size * sizeof(char));
	WideCharToMultiByte(CP_UTF8, 0, src, -1, m_buffer, m_size, 0, 0);
}
#endif
#endif
