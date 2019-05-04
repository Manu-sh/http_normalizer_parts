#include <http_parts/public/http_parts.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <regex>
#include <iterator>
#include <cassert>
#include <memory>

class http_url {

	inline bool has(const std::shared_ptr<const std::string> &shp) const 
	{ return shp != nullptr; }

	public:
		explicit http_url(const std::string &u);

		http_url(const http_url &) = delete;
		http_url & operator=(const http_url &) = delete;

		bool has_proto()    const { return has(m_proto);    }
		bool has_hostname() const { return has(m_hostname); }
		bool has_path()     const { return has(m_path);     }
		bool has_query()    const { return has(m_query);    }

		std::shared_ptr<const std::string> get_proto()    const { return m_proto;    }
		std::shared_ptr<const std::string> get_hostname() const { return m_hostname; }
		std::shared_ptr<const std::string> get_port()     const { return m_port;     }
		std::shared_ptr<const std::string> get_path()     const { return m_path;     }
		std::shared_ptr<const std::string> get_query()    const { return m_query;    }

	private:
		std::shared_ptr<std::string> m_url;
		std::shared_ptr<std::string> m_proto;
		std::shared_ptr<std::string> m_hostname;
		std::shared_ptr<std::string> m_port;
		std::shared_ptr<std::string> m_path;
		std::shared_ptr<std::string> m_query;
};

/* less variance is preferred here instead of major tolerance

	so for example using "test.com::90?ad" the following regex
	as path_reg produce differents results:

		^([^:\\?#]+)  -> http://test.com (less variance avoid to make useless http requests while crawling)
		^:*([^\\?#]+) -> http://test.com/90/?ad
		^([^\\?#]+)   -> http://test.com/%3A%3A90/?ad

	so the interpretation of where a part start is a detail that you must define
	based on your preference, for example, perhaps you prefer not to use regex,
	or you want be more strict on your input and so on.

	the previous input "test.com::90?ad" could be interpreted as http://test.com:90/?ad
	or could be treated like an error
*/
http_url::http_url(const std::string &u): m_url{std::make_shared<std::string>(u)} {

	using std::regex, std::regex_iterator, std::sregex_iterator, std::string, std::invalid_argument, std::to_string;

	static const auto &sh_str = [] (std::string &&s) { return s.empty() ? nullptr : std::make_shared<std::string>(s); };
 	static const auto proto_reg    = regex{"^([^://]{0,20})://"}; // rules to identify the proto part to normalize
	static const auto hostname_reg = regex{"^([^:/\\?]+)"};
	static const auto port_reg     = regex{"^:(\\d{1,8})"};
	static const auto path_reg     = regex{"^([^:\\?#]+)"};
	static const auto query_reg    = regex{"^\\?([^#/]+)"};

	static const auto URL_MAX = 2048;
	static const auto end = sregex_iterator{};

	if (m_url->length() > URL_MAX)
		throw invalid_argument("too long");

	auto from = m_url->begin();
	bool is_tls = false;

	auto match = sregex_iterator{(const string::const_iterator &)(from), m_url->end(), proto_reg};
	if (match != end) {
		from += match->length();
		if (match->str() != "://") {
			m_proto = sh_str(http_parts::normalize_protocol( (*match)[1].str() ));
			if (!m_proto) throw invalid_argument("invalid or missing protocol");
			is_tls = (m_proto && *m_proto == "https");
		}
	}

	// use http as protocol if no protocol is found but there is a valid hostname
	m_proto = sh_str("http");

	match = sregex_iterator{(const string::const_iterator &)(from), m_url->end(), hostname_reg};
	if (match != end) {
		from += match->length();
		m_hostname = sh_str(http_parts::normalize_hostname( (*match)[1].str() ));
	} else {
		throw invalid_argument("invalid or missing hostname");
	}

	match = sregex_iterator{(const string::const_iterator &)(from), m_url->end(), port_reg};
	if (match != end) {
		from += match->length();
		int tmp = http_parts::normalize_port( (*match)[1].str(), is_tls );
		if (tmp < 0) throw invalid_argument("invalid port");
		m_port  = tmp > 0 ? sh_str(to_string(tmp)) : m_port;
	}

	match = sregex_iterator{(const string::const_iterator &)(from), m_url->end(), path_reg};
	if (match != end) {
		from += match->length();
		m_path = sh_str(http_parts::normalize_path( (*match)[1].str() ));
	}

	match = sregex_iterator{(const string::const_iterator &)(from), m_url->end(), query_reg};
	if (match != end) {
		from += match->length();
		m_query = sh_str(http_parts::normalize_query( (*match)[1].str() ));
	}

	// fragments are ignored
}

/* reconstruct an url */
std::string get_url(const http_url &obj) noexcept {

	using std::literals::string_view_literals::operator""sv;

	static const std::initializer_list<std::pair<std::string_view, decltype(&http_url::get_proto)>> pairs{
		{""sv,    &http_url::get_proto}, /* string to prepend, function to call */
		{"://"sv, &http_url::get_hostname}, 
		{":"sv,   &http_url::get_port},
		{"/"sv,   &http_url::get_path},
		{"/?"sv,  &http_url::get_query}
	};

	std::ostringstream ret;

	try {
		for (const auto &pair : pairs)
			if (auto cb = pair.second; (obj.*cb)() != nullptr) 
				ret << pair.first << *(obj.*cb)();

	} catch (...) { return ""; }

	return ret.str();
}

std::string get_info(const http_url &obj) {

	static const std::initializer_list<std::pair<std::string_view, decltype(&http_url::get_proto)>> pairs{
		 {"scheme:   ", &http_url::get_proto}, 
		 {"hostname: ", &http_url::get_hostname}, 
		 {"port:     ", &http_url::get_port},
		 {"path:     ", &http_url::get_path}, 
		 {"query:    ", &http_url::get_query}
	};

	std::ostringstream ret;

	for (const auto &pair : pairs)
		if (const auto &cb = pair.second; (obj.*cb)() )
			ret << "function (" << &cb << ") " 
				<< pair.first << *(obj.*cb)() << '\n';

	ret << "normalized -> " << get_url(obj) << '\n';
	return ret.str();
}

using namespace std;
int main(int argc, const char *argv[]) {

	const char *defaults[] = {argv[0], "http://google.com"};
	argv = argc > 1 ? argv : (argc = std::size(defaults), defaults);

	for (int i = 1; i < argc; i++) {
		try { cout << "[" << argv[i] << "]\n" << get_info(http_url(argv[i])) << '\n';} 
		catch (const std::exception &ex) { cerr << argv[i] << ": " << ex.what() << endl;}
	}

}