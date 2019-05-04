cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug)
endif()

set(TEST_BIN ${PROJECT_NAME}_test)
add_executable(${TEST_BIN} ${PROJECT_NAME}_test.cpp)

target_link_libraries(${TEST_BIN} PRIVATE ${PROJECT_NAME})
target_compile_options(${PROJECT_NAME}
	PRIVATE
		-Wall -Wextra -Wno-unused-function -pipe -pedantic
		-fstack-protector-all -fstack-protector-strong -O0
)

add_test(
	NAME ${TEST_BIN}
	COMMAND ${EXECUTABLE_OUTPUT_PATH}/${TEST_BIN}
)