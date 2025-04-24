cmake_minimum_required (VERSION 3.21)

set(TARGET_NAME libpjson)

project (${TARGET_NAME})

# Project directories
set (SRC_DIR "src")
set (INCLUDE_DIR "include")

# Project Src files
set (SRC_FILES ${SRC_FILES}
${SRC_DIR}/pjson.cpp
)

# Project Include directories
set (INC_DIRS ${INC_DIR}
${CMAKE_CURRENT_SOURCE_DIR}
${INCLUDE_DIR}
)

# Compiler Flags
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Execute
add_library(${TARGET_NAME} ${SRC_FILES})
target_include_directories(${TARGET_NAME} PUBLIC ${INC_DIRS})
