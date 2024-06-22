cmake_minimum_required(VERSION 3.6)

project(s2)

set(CMAKE_CXX_STANDARD 11)

include_directories(
	scratch2
)

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_executable(s2tests
  scratch2/s2test.h
  scratch2/s2string.h
  scratch2/s2stringpath.h
  scratch2/s2list.h
  scratch2/s2dict.h
  scratch2/s2hashtable.h
  scratch2/s2set.h
  scratch2/s2file.h
  scratch2/s2ref.h
  scratch2/s2func.h

	tests/tests.cpp
	tests/impl.cpp
	tests/structs.cpp
	tests/test_string.cpp
	tests/test_stringpath.cpp
	tests/test_list.cpp
	tests/test_dict.cpp
	tests/test_hashtable.cpp
	tests/test_set.cpp
	tests/test_file.cpp
	tests/test_ref.cpp
	tests/test_func.cpp
)
