# this is a build using gnu-make (TODO: finish)

CXXFLAGS=-std=c++17 -Wall -Wextra -Wno-unused-function -pipe -pedantic -fstack-protector-all -fstack-protector-strong -O3 -ffast-math -funroll-loops -march=native
.PHONY: all clean

all:
	cd ../http_normalizer && mkdir -p build && cd build && cmake .. -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release && make
	$(CXX) $(CXXFLAGS) -o main main.cpp -I../http_normalizer/include/public ../http_normalizer/lib/libhttpnormalizer.a ../http_parts/lib/libhttparts.a
	strip --strip-all main

clean:
	rm -rf ../http_normalizer/lib/*
	rm -rf ../http_parts/lib/*
	rm -rf ../http_normalizer/build
	rm -rf main