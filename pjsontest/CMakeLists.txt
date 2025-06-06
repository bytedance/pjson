﻿cmake_minimum_required(VERSION 3.21)

set (TARGET_NAME pjsontest)

project (${TARGET_NAME})

# Project directories
set (SRC_DIR "src")
set (INCLUDE_DIR "include")

# Project Src files
set (SRC_FILES ${SRC_FILES}
${SRC_DIR}/main.cpp
)

# Project external libs
set(${TARGET_NAME}_libs
    "pjson"
	)

# Project Include directories
set (INC_DIRS ${INC_DIR}
${CMAKE_CURRENT_SOURCE_DIR}
${INCLUDE_DIR}
"../pjsonlib/include"
)

# Compiler Flags
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# Execute
add_executable(${TARGET_NAME} ${SRC_FILES})
target_include_directories(${TARGET_NAME} PUBLIC ${INC_DIRS})
target_link_libraries(${TARGET_NAME} ${${TARGET_NAME}_libs})
