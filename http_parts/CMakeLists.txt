project(httparts)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(INC_PATH  ${PROJECT_SOURCE_DIR}/include)
set(SRC_PATH  ${PROJECT_SOURCE_DIR}/src)

add_library(${PROJECT_NAME} STATIC ${SRC_PATH}/http_parts.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_PATH}/public)
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_PATH}/private)

target_compile_options(${PROJECT_NAME}
	PRIVATE
		-Wall -Wextra -Wno-unused-function -pipe -pedantic
		-fstack-protector-all -fstack-protector-strong
)

# CMAKE_CXX_FLAGS_RELEASE == Release ? "-O3 .." : ""
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-O3 -ffast-math -funroll-loops -march=native>)
target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:-O0 -g -DDEBUG>)

include(CTest)
if(BUILD_TESTING)
	message("Config build type: ${CMAKE_BUILD_TYPE}")
	add_subdirectory(tests)
endif()