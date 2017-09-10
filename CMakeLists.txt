cmake_minimum_required(VERSION 3.6)

project(s2)

set(CMAKE_CXX_STANDARD 11)

include_directories(
	scratch2
)

if(APPLE)
	add_definitions(-D_XOPEN_SOURCE)
endif()

add_executable(s2tests
	tests/tests.cpp
	tests/impl.cpp
	tests/structs.cpp
	tests/test_string.cpp
	tests/test_list.cpp
	tests/test_dict.cpp
	tests/test_file.cpp
	tests/test_ref.cpp
	tests/test_func.cpp
	tests/test_fiber.cpp
)
