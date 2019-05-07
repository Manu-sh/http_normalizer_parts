#include <http_parts/public/http_parts.hpp>
#include "http_tokenizer.hpp"

#include <sstream>
#include <iostream>
#include <string>
#include <string_view>
#include <stdexcept>
#include <iterator>
#include <memory>

class http_url { /* http url parser/normalizer */

	inline bool has(const std::shared_ptr<const std::string> &shp) const { return shp != nullptr; }
	friend std::ostream & operator<<(std::ostream &os, const http_url &obj);

	public:
		explicit http_url(const std::string &u);

		http_url(const http_url &) = delete;
		http_url & operator=(const http_url &) = delete;

		bool has_proto()    const { return has(m_proto);    }
		bool has_hostname() const { return has(m_hostname); }
		bool has_path()     const { return has(m_path);     }
		bool has_query()    const { return has(m_query);    }
		bool has_fragment() const { return has(m_fragment); }

		std::shared_ptr<const std::string> get_proto()    const { return m_proto;    }
		std::shared_ptr<const std::string> get_hostname() const { return m_hostname; }
		std::shared_ptr<const std::string> get_port()     const { return m_port;     }
		std::shared_ptr<const std::string> get_path()     const { return m_path;     }
		std::shared_ptr<const std::string> get_query()    const { return m_query;    }
		std::shared_ptr<const std::string> get_fragment() const { return m_fragment; }

	private:
		std::shared_ptr<std::string> m_url;
		std::shared_ptr<std::string> m_proto;
		std::shared_ptr<std::string> m_hostname;
		std::shared_ptr<std::string> m_port;
		std::shared_ptr<std::string> m_path;
		std::shared_ptr<std::string> m_query;
		std::shared_ptr<std::string> m_fragment;
};

http_url::http_url(const std::string &u): m_url{std::make_shared<std::string>(u)} {

	using std::string, std::invalid_argument, std::to_string;
	using std::cout, std::endl;

	static const auto &sh_str = [] (std::string &&s) { return s.empty() ? nullptr : std::make_shared<std::string>(s); };
	static const auto URL_MAX = 2048;

	if (m_url->length() > URL_MAX)
		throw invalid_argument("too long");


	const auto &parts = http_tokenizer(*m_url).m_vct;
	std::string tmp = parts[http_tokenizer::parts::PROTO].str();
	bool is_tls = false;

	if (tmp.empty()) {

		// use http as protocol if no protocol is found but there is a valid hostname
		m_proto = sh_str("http");

	} else {

		if (!(m_proto = sh_str(http_parts::normalize_protocol(tmp))))
			throw invalid_argument("invalid or missing protocol");

		is_tls = *m_proto == "https";
	}

	if (!(m_hostname = sh_str(http_parts::normalize_hostname(parts[http_tokenizer::parts::HOSTNAME].str()))))
		throw invalid_argument("invalid or missing hostname");

	if (!(tmp = parts[http_tokenizer::parts::PORT].str()).empty()) {
		int int_tmp = http_parts::normalize_port(tmp, is_tls);
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
std::string get_url(const http_url &obj) noexcept {

	using std::literals::string_view_literals::operator""sv;

	static const std::initializer_list<std::pair<std::string_view, decltype(&http_url::get_proto)>> pairs{
		{""sv,    &http_url::get_proto}, /* string to prepend, function to call */
		{"://"sv, &http_url::get_hostname}, 
		{":"sv,   &http_url::get_port},
		{"/"sv,   &http_url::get_path},
		{"/?"sv,  &http_url::get_query}
		/* fragments are ignored */
	};

	std::ostringstream ret;

	try {
		for (const auto &pair : pairs)
			if (auto cb = pair.second; (obj.*cb)() != nullptr) 
				ret << pair.first << *(obj.*cb)();

	} catch (...) { return ""; }

	return ret.str();
}

/* get all infos */
std::string get_info(const http_url &obj) {

	static const std::initializer_list<std::pair<std::string_view, decltype(&http_url::get_proto)>> pairs{
		 {"scheme:   ", &http_url::get_proto}, 
		 {"hostname: ", &http_url::get_hostname}, 
		 {"port:     ", &http_url::get_port},
		 {"path:     ", &http_url::get_path}, 
		 {"query:    ", &http_url::get_query},
		 {"fragment: ", &http_url::get_fragment}
	};

	std::ostringstream ret;

	for (const auto &pair : pairs)
		if (const auto &cb = pair.second; (obj.*cb)() )
			ret << "function (" << &cb << ") " 
				<< pair.first << *(obj.*cb)() << '\n';

	ret << "normalized -> " << get_url(obj) << '\n';
	return ret.str();
}

std::ostream & operator<<(std::ostream &os, const http_url &obj) {
	return os << get_info(obj); // be verbose
}

using namespace std;
int main(int argc, const char *argv[]) {

	const char *defaults[] = {argv[0], "http://hello.com:80/sad?x=2#sa"};
	argv = argc > 1 ? argv : (argc = std::size(defaults), defaults);

	if (argv[1] == defaults[1]) { // same address
		cout << "usage: " << argv[0] << " [example.com]\nstarting using the default argument \""
		<< argv[1] << "\"\n" << endl;
	}

	for (int i = 1; i < argc; i++) {
		try { cout << "[" << argv[i] << "]\n" << http_url(argv[i]) << '\n';} 
		catch (const std::exception &ex) { cerr << argv[i] << ": " << ex.what() << endl;}
	}

}