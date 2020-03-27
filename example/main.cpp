#include <http_normalizer/public/http_normalizer.hpp>

#include <iostream>
#include <iterator>
#include <stdexcept>

using namespace std;
int main(int argc, const char *argv[]) {

	const char *defaults[] = {argv[0], "http://hello.com:80/sad?x=2#sa"};
	argv = argc > 1 ? argv : (argc = std::size(defaults), defaults);

	if (argv[1] == defaults[1]) { // same address
		cout << "usage: " << argv[0] << " [example.com]\nstarting using the default argument \""
		<< argv[1] << "\"\n" << endl;
	}

	int flags = http_normalizer::OPT::HOSTNAME_STRIP_ALL_PREFIX_WWW | http_normalizer::OPT::PATH_REMOVE_DIRECTORY_INDEX;

	for (int i = 1; i < argc; i++) {
		try { cout << "[" << argv[i] << "]\n" << http_normalizer(argv[i], flags).dbg_info() << '\n';} 
		catch (const std::exception &ex) { cerr << argv[i] << ": " << ex.what() << endl;}
	}

}