#include <http_normalizer/public/http_normalizer.hpp>
#include <http_normalizer/private/http_tokenizer.hpp>
#include <http_parts/public/http_parts.hpp>

#include <sstream>
#include <iostream>
#include <string_view>
#include <stdexcept>
#include <iterator>


http_normalizer::http_normalizer(const std::string &u): m_url{std::make_shared<std::string>(u)} {

	using std::string, std::invalid_argument, std::to_string;
	using std::cout, std::endl;

	static const auto &sh_str = [] (std::string &&s) { return s.empty() ? nullptr : std::make_shared<std::string>(s); };
	static const auto URL_MAX = 2048;

	if (m_url->length() > URL_MAX)
		throw invalid_argument("too long");


	const auto &parts = http_tokenizer(*m_url).m_vct;
	std::string tmp = parts[http_tokenizer::parts::PROTO].str();

	if (tmp.empty()) {

		// use http as protocol if no protocol is found but there is a valid hostname
		m_proto = sh_str("http");

	} else {

		if (!(m_proto = sh_str(http_parts::normalize_protocol(tmp))))
			throw invalid_argument("invalid or missing protocol");

		m_is_https = *m_proto == "https";
	}

	if (!(m_hostname = sh_str(http_parts::normalize_hostname(parts[http_tokenizer::parts::HOSTNAME].str()))))
		throw invalid_argument("invalid or missing hostname");

	if (!(tmp = parts[http_tokenizer::parts::PORT].str()).empty()) {
		int int_tmp = http_parts::normalize_port(tmp, m_is_https);
		if (int_tmp < 0) throw invalid_argument("invalid port");
		m_port = int_tmp > 0 ? sh_str(to_string(int_tmp)) : m_port;
	}

	if (!(tmp = parts[http_tokenizer::parts::PATH].str()).empty())
		m_path  = sh_str( http_parts::normalize_path(tmp) );

	if (!(tmp = parts[http_tokenizer::parts::QUERY].str()).empty())
		m_query = sh_str( http_parts::normalize_query(tmp) );

	// fragments usually are ignored
	if (!(tmp = parts[http_tokenizer::parts::FRAGMENT].str()).empty())
		m_fragment = sh_str( std::move(tmp) );
}

/* get only the normalized url */
std::shared_ptr<const std::string> http_normalizer::normalized() const {

	using std::literals::string_view_literals::operator""sv;
	static const auto &sh_str = [] (std::string &&s) { return s.empty() ? nullptr : std::make_shared<std::string>(s); };

	static const std::initializer_list<std::pair<std::string_view, decltype(&http_normalizer::proto)>> pairs{
		{""sv,    &http_normalizer::proto}, /* string to prepend, function to call */
		{"://"sv, &http_normalizer::hostname}, 
		{":"sv,   &http_normalizer::port},
		{"/"sv,   &http_normalizer::path},
		{"/?"sv,  &http_normalizer::query}
		/* fragments are ignored */
	};

	if (m_normalized != nullptr)
		return m_normalized;

	std::ostringstream ret;

	try {
		for (const auto &pair : pairs)
			if (auto cb = pair.second; (*this.*cb)() != nullptr) 
				ret << pair.first << *(*this.*cb)();
	} catch (...) { return nullptr; }

	return (m_normalized = sh_str( ret.str() ));
}

http_normalizer & http_normalizer::try_parse(const std::string &u) {
	this->~http_normalizer(); /* reuse this memory */
	return new (this) http_normalizer(u), *this;
}

std::shared_ptr<const std::string> http_normalizer::normalize(const std::string &u) noexcept try {
	return this->try_parse(u).normalized();
} catch (...) {
	return nullptr;
}

/* get all infos */
std::string http_normalizer::dbg_info() const {

	static const std::initializer_list<std::pair<std::string_view, decltype(&http_normalizer::proto)>> pairs{
		 {"scheme:   ", &http_normalizer::proto}, 
		 {"hostname: ", &http_normalizer::hostname}, 
		 {"port:     ", &http_normalizer::port},
		 {"path:     ", &http_normalizer::path}, 
		 {"query:    ", &http_normalizer::query},
		 {"fragment: ", &http_normalizer::fragment}
	};

	std::ostringstream os; 
	for (const auto &pair : pairs)
		if (const auto &cb = pair.second; (*this.*cb)() )
			os << "function (" << &cb << ") " 
				<< pair.first << *(*this.*cb)() << '\n';

	return (os << "normalized -> " << *this->normalized() << '\n'), os.str();
}