# CXXFLAGS=-std=c++17 -pipe -Wall -Wextra -Wno-unused-function -fstack-protector-strong -pedantic -O0 # -O3 -ffast-math -funroll-loops -march=native
# LDLIBS=http_parts/build/libhttparts.a
.PHONY: all clean

all:
	mkdir -p http_parts/build && cd http_parts/build && cmake .. && make -j`nproc --all` && ctest -VV && cd -
	# $(CXX) $(CXXFLAGS) -o main main.cpp $(LDLIBS)
	# strip --strip-all main

clean:
	rm -rf http_parts/build
	rm -f main