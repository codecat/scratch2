#pragma once

#define S2_USING_PATH

#include "s2string.h"

namespace s2
{
	namespace path
	{
		// Gets the file extension for the given path, including the period. If there is no file extension,
		// this returns an empty string. For example, `hello.txt` will return `.txt`.
		s2::string getExtension(const s2::stringview& path);

		// Returns true if the given path has a file extension.
		bool hasExtension(const s2::stringview& path);

		// Changes the file extension in the given path and returns the new path. If the original path has
		// no file extension, this will add the given file extension.
		// - path: The original path.
		// - extension: The new extension for the given path. This does not need to start with a period, but
		// including a period is accepted.
		s2::string changeExtension(const s2::stringview& path, const s2::stringview& extension);

		// Removes the file extension from the given path. If there is no file extension, this returns the
		// original path. For example, `hello.txt` will return `hello`.
		s2::string removeExtension(const s2::stringview& path);

		// Combines two paths into one. This automatically glues the paths with forward slashes where needed.
		// For example, passing `hello` and `world` will return `hello/world`, but so will passing `hello/`
		// and `world`. You should not combine multiple absolute paths using this function.
		s2::string join(const s2::stringview& pathA, const s2::stringview& pathB);

		// Returns true if the given 2 paths can be considered equal.
		// - caseSensitive: Whether to test for case sensitivity.
		bool equals(const s2::stringview& pathA, const s2::stringview& pathB, bool caseSensitive);

		// Returns the path to the directory of the containing path, including the path separator, excluding
		// the filename. For example, `hello/world/foo.txt` will return `hello/world/`.
		s2::string getDirectoryName(const s2::stringview& path);

		// Gets the file name and extension of the given path. For example, `hello/world/foo.txt` will return
		// `foo.txt`.
		s2::string getFileName(const s2::stringview& path);

		// Gets the file name of the given path without the extension. For example, `hello/world/foo.txt` will
		// return `foo`.
		s2::string getFileNameWithoutExtension(const s2::stringview& path);
	}
}

#if defined(S2_IMPL)
#include <cstring>
#include <cctype>

s2::string s2::path::getExtension(const s2::stringview& path)
{
	const char* pStart = path.c_str();
	const char* pEnd = pStart + path.len();

	const char* pSlash = strrchr(pStart, '/');
	const char* pBackslash = strrchr(pStart, '\\');
	if (pBackslash != nullptr && pBackslash > pSlash) {
		pSlash = pBackslash;
	}

	const char* pPeriod = strrchr(pSlash != nullptr ? pSlash : pStart, '.');

	if (pPeriod != nullptr) {
		return s2::string(pPeriod, pEnd - pPeriod);
	}
	return "";
}

bool s2::path::hasExtension(const s2::stringview& path)
{
	const char* pStart = path.c_str();

	const char* pSlash = strrchr(pStart, '/');
	const char* pBackslash = strrchr(pStart, '\\');
	if (pBackslash != nullptr && pBackslash > pSlash) {
		pSlash = pBackslash;
	}

	return strrchr(pSlash != nullptr ? pSlash : pStart, '.') != nullptr;
}

s2::string s2::path::changeExtension(const s2::stringview& path, const s2::stringview& extension)
{
	const char* pStart = path.c_str();
	const char* pEnd = pStart + path.len();

	const char* pSlash = strrchr(pStart, '/');
	const char* pBackslash = strrchr(pStart, '\\');
	if (pBackslash != nullptr && pBackslash > pSlash) {
		pSlash = pBackslash;
	}

	const char* pPeriod = strrchr(pSlash != nullptr ? pSlash : pStart, '.');

	s2::string ret;
	if (pPeriod == nullptr) {
		// There is no extension in the path, so we can add the entire thing w/o any extension and period here
		ret.append(pStart, pEnd - pStart);

		// Add a period if it's not already in the extension
		if (extension.len() == 0 || extension[0] != '.') {
			ret.append('.');
		}

		// Add the new extension
		if (extension.len() > 0) {
			ret.append(extension);
		}
	} else {
		// There is an extension in the path, so add it here, including the period for the extension
		ret.append(pStart, (pPeriod - pStart) + 1);

		// We don't have to add the period, so skip over it if it's in the extension
		if (extension.len() > 0 && extension[0] == '.') {
			ret.append(extension.c_str() + 1, extension.len() - 1);
		} else {
			ret.append(extension);
		}
	}

	return ret;
}

s2::string s2::path::removeExtension(const s2::stringview& path)
{
	const char* pStart = path.c_str();
	const char* pEnd = pStart + path.len();

	const char* pSlash = strrchr(pStart, '/');
	const char* pBackslash = strrchr(pStart, '\\');
	if (pBackslash != nullptr && pBackslash > pSlash) {
		pSlash = pBackslash;
	}

	const char* pPeriod = strrchr(pSlash != nullptr ? pSlash : pStart, '.');

	if (pPeriod == nullptr) {
		// There is no extension in the path, so we can add the entire thing w/o any extension and period here
		return s2::string(pStart, pEnd - pStart);
	}

	// There is an extension in the path, so add it here, excluding the period for the extension
	return s2::string(pStart, pPeriod - pStart);
}

s2::string s2::path::join(const s2::stringview& pathA, const s2::stringview& pathB)
{
	if (pathA.len() == 0) {
		return s2::string(pathB.c_str(), pathB.len());
	} else if (pathB.len() == 0) {
		return s2::string(pathA.c_str(), pathA.len());
	}

	s2::string ret(pathA);

	const char ca = pathA[pathA.len() - 1];
	if (ca != '/' && ca != '\\') {
		ret.append('/');
	}

	const char cb = pathB[0];
	if (cb == '/' || cb == '\\') {
		ret.append(pathB.c_str() + 1, pathB.len() - 1);
	} else {
		ret.append(pathB.c_str(), pathB.len());
	}

	return ret;
}

bool s2::path::equals(const s2::stringview& pathA, const s2::stringview& pathB, bool caseSensitive)
{
	if (pathA.len() != pathB.len()) {
		return false;
	}

	const char* pa = pathA.c_str();
	const char* pb = pathB.c_str();

	while (*pa != '\0' && *pb != '\0') {
		char ca, cb;
		if (caseSensitive) {
			ca = *pa;
			cb = *pb;
		} else {
			ca = tolower(*pa);
			cb = tolower(*pb);
		}

		if (ca == cb || (ca == '/' && cb == '\\') || (ca == '\\' && cb == '/')) {
			pa++;
			pb++;
		} else {
			return false;
		}
	}

	return true;
}

s2::string s2::path::getDirectoryName(const s2::stringview& path)
{
	const char* pStart = path.c_str();
	const char* pEnd = pStart + path.len();

	const char* pSlash = strrchr(pStart, '/');
	const char* pBackslash = strrchr(pStart, '\\');
	if (pBackslash != nullptr && pBackslash > pSlash) {
		pSlash = pBackslash;
	}

	if (pSlash != nullptr) {
		return s2::string(pStart, pSlash - pStart + 1);
	}
	return "";
}

s2::string s2::path::getFileName(const s2::stringview& path)
{
	const char* pStart = path.c_str();
	const char* pEnd = pStart + path.len();

	const char* pSlash = strrchr(pStart, '/');
	const char* pBackslash = strrchr(pStart, '\\');
	if (pBackslash != nullptr && pBackslash > pSlash) {
		pSlash = pBackslash;
	}

	if (pSlash != nullptr) {
		return s2::string(pSlash + 1, pEnd - (pSlash + 1));
	}
	return s2::string(path.c_str(), path.len());
}

s2::string s2::path::getFileNameWithoutExtension(const s2::stringview& path)
{
	const char* pStart = path.c_str();
	const char* pEnd = pStart + path.len();

	const char* pSlash = strrchr(pStart, '/');
	const char* pBackslash = strrchr(pStart, '\\');
	if (pBackslash != nullptr && pBackslash > pSlash) {
		pSlash = pBackslash;
	}

	const char* pPeriod = strrchr(pSlash != nullptr ? pSlash : pStart, '.');
	if (pPeriod != nullptr) {
		pEnd = pPeriod;
	}

	if (pSlash != nullptr) {
		return s2::string(pSlash + 1, pEnd - (pSlash + 1));
	}
	return s2::string(pStart, pEnd - pStart);
}

#endif
