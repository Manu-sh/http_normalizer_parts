#pragma once
#include <string>

namespace http_parts {
	std::string normalize_protocol(const std::string &proto) noexcept;      // "" on error
	std::string normalize_hostname(const std::string &hostname) noexcept;   // "" on error
	int normalize_port(const std::string &port, bool tls) noexcept;         // the port || 0 on default port || -1 on error
	std::string normalize_path(const std::string &path)   noexcept;         // no error are possible
	std::string normalize_query(const std::string &query) noexcept;         // no error are possible
}