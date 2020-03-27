#include <http_normalizer/public/http_normalizer.hpp>
#include <phpcpp.h>
#include <string>

Php::Value normalize_url(Php::Parameters &params) {

	constexpr static int HTTP_NORMALIZER_FLAGS = http_normalizer::OPT::HOSTNAME_STRIP_ALL_PREFIX_WWW | http_normalizer::OPT::PATH_REMOVE_DIRECTORY_INDEX;
    const static std::string instance;

    Php::Value string = params[0];
	const auto &ptr = http_normalizer::normalize(string, HTTP_NORMALIZER_FLAGS);
	return ptr ? *ptr : instance;
}

extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("libphp_http_normalize", "1.0");
        extension.add<normalize_url>("normalize_http_url", {
            Php::ByVal("input_url", Php::Type::String)
        });
        return extension;
    }
}
