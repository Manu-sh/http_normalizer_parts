# this is a build using cmake

.PHONY: all clean


NAME                            = libphp_http_normalize_parts

#       Php.ini directories
#
#       In the past, PHP used a single php.ini configuration file. Today, most
#       PHP installations use a conf.d directory that holds a set of config files,
#       one for each extension. Use this variable to specify this directory.

INI_DIR                         = /etc/php/conf.d


#       The extension dirs
#
#       This is normally a directory like /usr/lib/php5/20121221 (based on the
#       PHP version that you use. We make use of the command line 'php-config'
#       instruction to find out what the extension directory is, you can override
#       this with a different fixed directory

EXTENSION_DIR                   = $(shell php-config --extension-dir)


#       The name of the extension and the name of the .ini file
#       These two variables are based on the name of the extension. We simply add
#       a certain extension to them (.so or .ini)

EXTENSION                       = build/${NAME}.so
INI                             = ${NAME}.ini
CP                              = cp -f

all:
	mkdir -p build                                          && \
	cd build                                                && \
	cmake -DBUILD_TESTING=OFF -DCMAKE_BUILD_TYPE=Release .. && \
	make -j`nproc --all`

	sudo ${CP} ${EXTENSION} ${EXTENSION_DIR}
	sudo ${CP} ${INI} ${INI_DIR}

clean:
	rm -rf build
