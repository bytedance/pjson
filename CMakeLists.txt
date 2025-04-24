# Include sub-projects.
cmake_minimum_required (VERSION 3.21)

project(pjson LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

# Include sub-projects.
add_subdirectory ("pjsonlib")
add_subdirectory ("pjsontest")