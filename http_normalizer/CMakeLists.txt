project(httpnormalizer)
cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 17)

set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(INC_PATH  ${PROJECT_SOURCE_DIR}/include)
set(SRC_PATH  ${PROJECT_SOURCE_DIR}/src)

add_library(${PROJECT_NAME} STATIC ${SRC_PATH}/http_normalizer.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC ${INC_PATH}/public)
target_include_directories(${PROJECT_NAME} PRIVATE ${INC_PATH}/private)

# add & link http_parts
add_subdirectory(../http_parts 3party)
target_link_libraries(${PROJECT_NAME} PRIVATE httparts)

target_compile_options(${PROJECT_NAME}
	PRIVATE
		-Wall -Wextra -Wno-unused-function -pipe -pedantic
		-fstack-protector-all -fstack-protector-strong
		-O3 -ffast-math -funroll-loops -march=native
)