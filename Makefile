.PHONY: all clean

all:
	mkdir -p http_parts/build && cd http_parts/build && cmake -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Debug .. && make -j`nproc --all` && ctest -VV

clean:
	rm -rf http_parts/build