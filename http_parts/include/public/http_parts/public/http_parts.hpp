#pragma once
#include <string>

namespace http_parts {

	enum OPT: unsigned char {
		HOSTNAME_STRIP_ALL_PREFIX_WWW = 1, // normalize_hostname_opt
		PATH_REMOVE_DIRECTORY_INDEX   = 1 << 1
	};

	std::string normalize_protocol(const std::string &proto) noexcept; // "" on error
	std::string normalize_hostname(const std::string &hostname, int flags = OPT::HOSTNAME_STRIP_ALL_PREFIX_WWW) noexcept;   // "" on error
	int normalize_port(const std::string &port, bool is_tls) noexcept; // the port || 0 on default port || -1 on error

	std::string normalize_path(const std::string &path, int flags = OPT::PATH_REMOVE_DIRECTORY_INDEX) noexcept; // no error are possible
	std::string normalize_query(const std::string &query) noexcept; // no error are possible
}