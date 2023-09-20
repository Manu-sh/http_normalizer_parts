#include <http_parts/public/http_parts.hpp>
#include <http_parts/private/pct.hpp>
#include <http_parts/private/utils.hpp>

#include <map>
#include <cctype>
#include <cstring>
#include <algorithm>
#include <limits>
#include <iterator> // std::size

#ifdef DEBUG
	#include <iostream>
	#include <cassert>
	using std::cout, std::endl;
	// #warning "DEBUG MODE ON "
#else
	// #warning "DEBUG MODE OFF "
#endif

using namespace http_parts::pct;
using namespace http_parts::utils;

// empty strings are invalid labels
static inline bool check_label(const std::string &label) noexcept {

	/* each label must be end and start with alnum, '-' is allowed in the middle */
	// https://tools.ietf.org/html/rfc3986#page-21
	if (!isalnum(label[0]) || !isalnum(label.back()))
		return false;

	return label.length() <= 2 ? true : std::all_of(label.cbegin() + 1, label.cend() - 1, [] (char ch) {
		return isalnum(ch) || ch == '-';
	});
}

// return "" on error
std::string http_parts::normalize_protocol(const std::string &proto) noexcept {

	if (proto.length() > 15) return "";

	auto cpy = pec_normalize(proto, [] (char ch) {
		return !isalnum(ch) && !memchr("+-.", ch, 3);
	}, [] (unsigned char c) { return tolower(c); });

	return cpy == "http" || cpy == "https" ? cpy : "";
}

// return "" on error
std::string http_parts::normalize_hostname(const std::string &hostname, int flags) noexcept {

	// https://tools.ietf.org/html/rfc1123#page-13
	static const auto MAX_LABELS   = 20;
	static const auto HOSTNAME_MAX = 63;

	auto labels = split(hostname, ".");
	if (labels.empty()) return "";

	{
		int count = 0;
		for (auto &label : labels) {

			/* each label must be end and start with alnum, '-' is allowed in the middle */
			// (++count including www. labels so this can have an influence on MAX_LABEL)
			if (!check_label(label = pec_decode(label)) || ++count > MAX_LABELS) 
				return "";

			std::transform(label.cbegin(), label.cend(), label.begin(), tolower);
		}
	}

	if (flags & OPT::HOSTNAME_STRIP_ALL_PREFIX_WWW) {
		// remove all consecutive www. from start
		for (size_t i = 0; i < labels.size() && labels[i] == "www"; i++)
			labels[i] = "";
	}

	const auto &ret = join(labels, ".");

	// at least 2 labels || die, so domains like localhost will be rejected
	return ret.length() > HOSTNAME_MAX || ret.find('.') == std::string::npos ? "" : ret;
}

// the port || 0 on default port || -1 on error (pct-encoded port are not allowed)
int http_parts::normalize_port(const std::string &port, bool is_tls) noexcept {

	if (!std::all_of(port.cbegin(), port.cend(), isdigit))
		return -1;

	try {
		// remove default ports
		if (const unsigned long x = stoul(port); x <= std::numeric_limits<unsigned short>::max())
			return (x == 80 && !is_tls) || (x == 443 && is_tls) ? 0 : ((unsigned short)x) > 0 ? ((unsigned short)x) : -1;
	} catch (...) { }

	return -1;
}

// no errors are possible
std::string http_parts::normalize_path(const std::string &path, int flags) noexcept {

	// Removing duplicate slashes: (change semantics)

	auto segments = split(path, "/");
    if (segments.empty()) return (flags & PATH_KEEP_TRAILING_SLASH) ? path : "";

	for (auto &seg : segments) {

		// decode unreserved characters & encode reserved characters
		seg = pec_normalize(seg, [] (char ch) {
			static const char unreserved[] = "-._~" "!$&'()*+,";
			static const unsigned char unreserved_len = std::size(unreserved) - 1; /* without '\0' */
			return !isalnum(ch) && !memchr(unreserved, ch, unreserved_len);
		});

		// remove dot segments: https://tools.ietf.org/html/rfc3986#page-33
		if (seg == "." || seg == "..")
			seg = "";
	}

	// removing directory index (semantic change)
	if (flags & OPT::PATH_REMOVE_DIRECTORY_INDEX) {
		for (const char *s : {"index.html", "index.php", "default.asp", "index.shtml", "index.jsp"}) {
			if (segments.back() == s) {
				segments.pop_back();
				break;
			}
		}
	}

    auto retval = join(segments, "/");
    if (!(flags & PATH_KEEP_TRAILING_SLASH) || path.back() != '/')
        return retval;

    return retval.push_back('/'), retval;
}

// no errors are possible
std::string http_parts::normalize_query(const std::string &query) noexcept {

	// decode unreserved characters & encode reserved characters
	static const auto &is_reserved = [] (char ch) {
        static const char unreserved[] = "!$'/()*,;:@-._~";
		static const unsigned char unreserved_len = std::size(unreserved) - 1; /* without '\0' */
		return !isalnum(ch) && !memchr(unreserved, ch, unreserved_len);
	};

	auto cpy = find_and_replace(query, "+", "%20"); // first convert all spaces into a %20
	auto vkvs = split(cpy, "&");

	// sorting the query parameters (change semantic)
	std::map<std::string, std::string> sorted;

	if (vkvs.empty()) {
		// workaround to remove each '&' from string (example: "&&&&&&foo" become "======foo"
		// and will be processed normally into the loop so the resulted query will be "foo"
		std::replace(cpy.begin(), cpy.end(), '&', '=');
		vkvs.push_back(cpy);
	}

	for (const auto &kvs : vkvs) {
		auto kv = split(kvs, "=");
		// if the pair is incomplete (there the key but not the value)
		// example: &key_and_value=his_value&only_key=&always_only_key
		if (kv.size() < 2) { if (kv.size() == 1) kv.emplace_back(""); else continue; } 

		// pct_normalize every key and value an insert them into the sorted map
		sorted.insert_or_assign(pec_normalize(kv[0], is_reserved), pec_normalize(kv[1], is_reserved));
	}

	vkvs.clear();
	for (const auto &pair : sorted)
		vkvs.push_back(pair.second.empty() ? pair.first : pair.first + '=' + pair.second); // if k has a value

	return vkvs.empty() ? "" : find_and_replace( (cpy = join(vkvs, "&")), "%20", "+"); // convert all previously %20 encoded spaces into a + encoded spaces
}
