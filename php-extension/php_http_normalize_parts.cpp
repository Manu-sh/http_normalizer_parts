
#include <http_parts/public/http_parts.hpp>

#include <phpcpp.h>
#include <string>

constexpr static int HTTP_NORMALIZER_FLAGS = http_parts::OPT::HOSTNAME_STRIP_ALL_PREFIX_WWW | http_parts::OPT::PATH_REMOVE_DIRECTORY_INDEX;

Php::Value http_normalize_proto(Php::Parameters &params) {
    Php::Value string_val = params[0];
	return http_parts::normalize_protocol(string_val);
}

Php::Value http_normalize_hostname(Php::Parameters &params) {
    Php::Value string_val = params[0];
	return http_parts::normalize_hostname(string_val, ::HTTP_NORMALIZER_FLAGS);
}

Php::Value http_normalize_port(Php::Parameters &params) {
    Php::Value string_val = params[0];
    Php::Value   bool_val = params[1];
	return http_parts::normalize_port(string_val, bool_val);
}

Php::Value http_normalize_path(Php::Parameters &params) {
    Php::Value string_val = params[0];
	return http_parts::normalize_path(string_val);
}

Php::Value http_normalize_query(Php::Parameters &params) {
    Php::Value string_val = params[0];
	return http_parts::normalize_query(string_val);
}

extern "C" {
    PHPCPP_EXPORT void *get_module() {
        static Php::Extension extension("libphp_http_normalize_parts", "1.0");
        
        extension.add<http_normalize_proto>("http_normalize_proto", {
            Php::ByVal("input_part", Php::Type::String, true)
        });

        extension.add<http_normalize_hostname>("http_normalize_hostname", {
            Php::ByVal("input_part", Php::Type::String, true)
        });

        extension.add<http_normalize_port>("http_normalize_port", {
            Php::ByVal("input_part", Php::Type::Numeric, true),
            Php::ByVal("is_tls", Php::Type::Bool)
        });

        extension.add<http_normalize_path>("http_normalize_path", {
            Php::ByVal("input_part", Php::Type::String, true)
        });

        extension.add<http_normalize_query>("http_normalize_query", {
            Php::ByVal("input_part", Php::Type::String, true)
        });

        // TODO: here
        // extension.add(Php::Constant("HTTP_PARTS_HOSTNAME_STRIP_ALL_PREFIX_WWW", http_parts::OPT::HOSTNAME_STRIP_ALL_PREFIX_WWW));
        // extension.add(Php::Constant("HTTP_PARTS_PATH_REMOVE_DIRECTORY_INDEX", http_parts::OPT::PATH_REMOVE_DIRECTORY_INDEX));

        return extension;
    }
}
