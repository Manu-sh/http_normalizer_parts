.PHONY: all clean

NAME                            = libphp_http_normalize_parts

#       Php.ini directories
#
#       In the past, PHP used a single php.ini configuration file. Today, most
#       PHP installations use a conf.d directory that holds a set of config files,
#       one for each extension. Use this variable to specify this directory.

INI_DIR                         = /etc/php/conf.d
EXTENSION_DIR                   = $(shell php-config --extension-dir)


#       The name of the extension and the name of the .ini file
#       These two variables are based on the name of the extension. We simply add
#       a certain extension to them (.so or .ini)

EXTENSION                       = build/${NAME}.so
INI                             = ${NAME}.ini

all:
	mkdir -p build                                          && \
	cd build                                                && \
	cmake -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release .. && \
	make -j`nproc --all`

install:
	sudo cp -f ${EXTENSION} ${EXTENSION_DIR}
	sudo cp -f ${INI} ${INI_DIR}

clean:
	rm -rf build
