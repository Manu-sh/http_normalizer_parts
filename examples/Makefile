.PHONY: all clean

all:
	mkdir -p build && cd build && cmake -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release .. && make -j`nproc --all` && strip --strip-all httpnormalizer
clean:
	rm -rf build