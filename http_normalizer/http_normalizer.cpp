#include <http_parts/public/http_parts.hpp>
#include "http_tokenizer.hpp"

#include <sstream>
#include <iostream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <iterator>
#include <memory>

class http_normalizer { /* http url parser/normalizer */

	protected:
		explicit http_normalizer(const std::string &u);
		http_normalizer(const http_normalizer &) = delete;
		http_normalizer & operator=(const http_normalizer &) = delete;

	public:
		http_normalizer() = default;

		/* mutators */
		http_normalizer & try_parse(const std::string &u);
		std::shared_ptr<const std::string> normalize(const std::string &u) noexcept;

		/* accessors */
		bool is_https() const { return m_is_https; }
		std::shared_ptr<const std::string> proto()    const { return m_proto;    }
		std::shared_ptr<const std::string> hostname() const { return m_hostname; }
		std::shared_ptr<const std::string> port()     const { return m_port;     }
		std::shared_ptr<const std::string> path()     const { return m_path;     }
		std::shared_ptr<const std::string> query()    const { return m_query;    }
		std::shared_ptr<const std::string> fragment() const { return m_fragment; }

		std::shared_ptr<const std::string> normalized() const;
		std::string dbg_info() const;

	private:
		mutable std::shared_ptr<std::string> m_normalized;
		std::shared_ptr<std::string> m_url;

		std::shared_ptr<std::string> m_proto;
		std::shared_ptr<std::string> m_hostname;
		std::shared_ptr<std::string> m_port;
		std::shared_ptr<std::string> m_path;
		std::shared_ptr<std::string> m_query;
		std::shared_ptr<std::string> m_fragment;
		bool m_is_https = false;
};

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

using namespace std;
int main(int argc, const char *argv[]) {

	const char *defaults[] = {argv[0], "http://hello.com:80/sad?x=2#sa"};
	argv = argc > 1 ? argv : (argc = std::size(defaults), defaults);

	if (argv[1] == defaults[1]) { // same address
		cout << "usage: " << argv[0] << " [example.com]\nstarting using the default argument \""
		<< argv[1] << "\"\n" << endl;
	}

	auto ht = http_normalizer();

	for (int i = 1; i < argc; i++) {
		try { cout << "[" << argv[i] << "]\n" << ht.try_parse(argv[i]).dbg_info() << '\n';} 
		catch (const std::exception &ex) { cerr << argv[i] << ": " << ex.what() << endl;}
	}

}