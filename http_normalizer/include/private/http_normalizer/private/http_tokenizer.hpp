#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <functional>
#include <sstream>

class http_tokenizer {

	friend class http_normalizer;
	enum parts: uint8_t { PROTO, HOSTNAME, PORT, PATH, QUERY, FRAGMENT, LENGTH };

	explicit http_tokenizer(const std::string &http_url);
	std::vector<std::ostringstream> m_vct;
};

http_tokenizer::http_tokenizer(const std::string &http_url): m_vct(parts::LENGTH) {

	using std::vector, std::string, std::function, std::pair;
	using functions = function<bool(const string &, size_t &, uint_fast8_t &)>;

	const static vector< functions > eval
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
				case '/': j = parts::PATH; return (i += i+1 < s.length()), true;
				case '?': j = parts::QUERY; return (i += i+1 < s.length()), true;
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
		}, [] ([[maybe_unused]] const string &s, [[maybe_unused]] size_t &i, [[maybe_unused]] uint_fast8_t &j) { // match fragment start
			return false; // NOP
		}
	};

	uint_fast8_t j = 0;

	// since protocol could be empty and all evaluation from eval
	// are performed one at time sequentially this is a workaround

	if (http_url.find("://") == std::string::npos)
		j = parts::HOSTNAME;

	for (size_t i = 0; i < http_url.length(); ++i) {
		while ( eval.at(j)(http_url, i, j) );
		m_vct.at(j).put(http_url.at(i));
	}

}