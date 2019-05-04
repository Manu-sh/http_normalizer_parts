#include <http_parts/public/http_parts.hpp>
#include <sstream>
#include <iostream>
#include <string>
#include <stdexcept>
#include <regex>
#include <iterator>
#include <cassert>
#include <memory>

class http_url {

	inline bool has(const std::shared_ptr<const std::string> &shp) const 
	{ return shp != nullptr && !shp.get()->empty(); }

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

http_url::http_url(const std::string &u): m_url{std::make_shared<std::string>(u)} {

	using std::regex, std::regex_iterator, std::sregex_iterator, std::string, std::invalid_argument, std::to_string;

	static const auto &sh_str = [] (std::string &&s) { return std::make_shared<std::string>(s); };
 	static const auto proto_reg    = regex{"^([^://]{1,20})://"};
	static const auto hostname_reg = regex{"([^:/\\?]+)"};
	static const auto port_reg     = regex{"^:(\\d{1,8})"};
	static const auto path_reg     = regex{"^:?/([^\\?#]+)"};
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
		m_proto = sh_str(http_parts::normalize_protocol( (*match)[1].str() ));
		is_tls = *m_proto == "https";
	} else {
		throw invalid_argument("invalid or missing protocol");
	}

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

using namespace std;
int main(int argc, const char *argv[]) {

	http_url test{ argc > 1 ? argv[1] : "http://google.com"};
	for (const auto &cb : {&http_url::get_proto, &http_url::get_hostname, &http_url::get_port, &http_url::get_path, &http_url::get_query})
		if ( (test.*cb)() ) cout << "function [" << &cb << "] " << *(test.*cb)() << endl;
}