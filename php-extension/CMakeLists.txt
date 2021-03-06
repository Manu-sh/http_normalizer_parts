project(php_http_normalize_parts)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH, ${PROJECT_SOURCE_DIR}/bin)

# equivalent of: strip --strip-all
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -s")

# enable/disable tests
# include(CTest)

# shlib
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_library(${PROJECT_NAME} SHARED ${PROJECT_NAME}.cpp)
# endshlib

# add & link http_parts
add_subdirectory(../http_parts 3party)
target_link_libraries(${PROJECT_NAME} PRIVATE httparts)

target_link_libraries(${PROJECT_NAME} PRIVATE phpcpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_options(${PROJECT_NAME}
	PRIVATE
		-Wall -Wextra -Wno-unused-function -pipe -pedantic
		-fstack-protector-all -fstack-protector-strong
		-O3 -ffast-math -funroll-loops -march=native
)



