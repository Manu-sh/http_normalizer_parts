#include <cstdint>
#include <vector>
#include <forward_list>
#include <string>
#include <functional>
#include <sstream>

class http_tokenizer {

	friend class http_url;
	enum parts: uint8_t { PROTO, HOSTNAME, PORT, PATH, QUERY, FRAGMENT, LENGTH };

	explicit http_tokenizer(const std::string &http_url);
	std::vector<std::ostringstream> m_vct;
};

http_tokenizer::http_tokenizer(const std::string &http_url): m_vct(parts::LENGTH) {

	using std::vector, std::string, std::forward_list, std::function, std::pair;
	using functions = function<bool(const string &, size_t &, uint_fast8_t &)>;

	const static forward_list< functions > prototype
	{
		[] (const string &s, size_t &i, uint_fast8_t &j) { // match proto end

			if (i+3 < s.length() && s[i] == ':' && s[i+1] == '/' && s[i+2] == '/') {
				j = parts::HOSTNAME;
				return i += 3, true;
			}

			return false;

		}, [] (const string &s, size_t &i, uint_fast8_t &j) { // match host end

			switch (s[i]) {
				case ':': j = parts::PORT;     return (i += i+1 < s.length()), true;
				case '/': j = parts::PATH;     return (i += i+1 < s.length()), true;
				case '?': j = parts::QUERY;    return (i += i+1 < s.length()), true;
				case '#': j = parts::FRAGMENT; return (i += i+1 < s.length()), true;
			}

			return false;

		}, [] (const string &s, size_t &i, uint_fast8_t &j) { // match port end

			switch (s[i]) {
				case '/': j = parts::PATH;     return (i += i+1 < s.length()), true;
				case '?': j = parts::QUERY;    return (i += i+1 < s.length()), true;
				case '#': j = parts::FRAGMENT; return (i += i+1 < s.length()), true;
			}

			return false;

		}, [] (const string &s, size_t &i, uint_fast8_t &j) { // match path end

			switch (s[i]) {
				case '?': j = parts::QUERY;    return (i += i+1 < s.length()), true;
				case '#': j = parts::FRAGMENT; return (i += i+1 < s.length()), true;
			}

			return false;

		}, [] (const string &s, size_t &i, uint_fast8_t &j) { // match query end

			switch (s[i]) {
				case '#': j = parts::FRAGMENT; return (i += i+1 < s.length()), true;
			}

			return false;
		}
	};

	// make a copy
	auto eval = prototype;
	uint_fast8_t j = 0;

	// since protocol could be empty and all evaluation from eval
	// are performed one at time sequentially this is a workaround

	if (http_url.find("://") == std::string::npos) {
		eval.pop_front();
		j = parts::HOSTNAME;
	}

	for (size_t i = 0; i < http_url.length(); ++i) {
		while ( !eval.empty() && eval.front()(http_url, i, j) )
			eval.pop_front();

		 m_vct.at(j).put(http_url.at(i));
	}

}