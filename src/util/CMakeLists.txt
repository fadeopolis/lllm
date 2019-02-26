
cmake_minimum_required(VERSION 3.11)

add_library( util fail.cpp InternedString.cpp SourceLocation.cpp EscapeStatus.cpp TypeSet.cpp util_io.cpp )

