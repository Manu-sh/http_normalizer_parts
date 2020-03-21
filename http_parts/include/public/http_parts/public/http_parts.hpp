#pragma once
#include <string>

namespace http_parts {

	enum OPT: unsigned char {
		HOSTNAME_STRIP_ALL_PREFIX_WWW = 1 // normalize_hostname_opt
	};

	std::string normalize_protocol(const std::string &proto) noexcept;      // "" on error
	std::string normalize_hostname(const std::string &hostname, int flag = OPT::HOSTNAME_STRIP_ALL_PREFIX_WWW) noexcept;   // "" on error
	int normalize_port(const std::string &port, bool tls) noexcept;         // the port || 0 on default port || -1 on error

	// TODO: options
	std::string normalize_path(const std::string &path)   noexcept;         // no error are possible

	// TODO: options
	std::string normalize_query(const std::string &query) noexcept;         // no error are possible
}