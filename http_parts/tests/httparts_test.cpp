#include <http_parts/public/http_parts.hpp>

#include <string>
#include <string_view>
#include <iostream>
#include <cassert>
#include <stdexcept>
#include <limits>

using namespace std;
using namespace std::literals;
using namespace http_parts;

static inline void expected(const std::string &test, const std::string &expected, const std::string &msg = "") {
	if (test != expected) throw std::invalid_argument( msg + "\nretval( "s + test + " ), expected ( "s + expected + " )"s );
}

static inline void expected(int test, int expected, const std::string &msg = "") {
	return ::expected(std::to_string(test), std::to_string(expected), msg);
}

static inline std::string msg(int line) {
	return __FILE__": "s + std::to_string(line);
}

int main() try {

	// protocol
	for (const auto &in : {"", "h", "%00", "hTt%50%00"})
		expected( normalize_protocol(in), "", msg(__LINE__) );

	for (const auto &in : {"hTt%50", "hTt%50"})
		expected( normalize_protocol(in), "http", msg(__LINE__) );

	for (const auto &in : {"hTt%50s", "hTt%50S", "hTt%50%53"})
		expected( normalize_protocol(in), "https", msg(__LINE__) );


	// hostname

	// at least 2 labels
	expected( normalize_hostname("at-least-2-labels-required"), "", msg(__LINE__) );

	// max 64 chars for hostname
	expected( normalize_hostname( string(61, '1') + "." + string(1, '1') ), string(61, '1') + "." + string(1, '1'), msg(__LINE__) );
	expected( normalize_hostname( string(61, '1') + "." + string(1, '1') ), string(61, '1') + "." + string(1, '1'), msg(__LINE__) );

	// max 20 labels
	expected( normalize_hostname("1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20.21"), "", msg(__LINE__) );
	expected( normalize_hostname("1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20"), "1.2.3.4.5.6.7.8.9.10.11.12.13.14.15.16.17.18.19.20", msg(__LINE__) );


	for (const auto &in : {"", "blah.-s-", "blah.s%2d", ".", "%2e", "%2E%2e", "...", "www", "www.", "localhost", "anyothersinglelabel", "%00", "%67.Com%00"})
		expected( normalize_hostname(in), "", msg(__LINE__) );

	for (const auto &in : {"G.com", "%47.com", "%67.Com", "www.G.com", "www.WWW.G.com", "%57ww.ww%77.G.cOm"})
		expected( normalize_hostname(in), "g.com", msg(__LINE__) );

	for (const auto &in : {"G.www.com", "%47.www.com", "%67.www.Com", "www.G.w%57w.com", "%57WW.www.G.WW%77.com"})
		expected( normalize_hostname(in), "g.www.com", msg(__LINE__) );

	for (const auto &in : {"www.label-ok.com", "www.label%2dok.com"})
		expected( normalize_hostname(in), "label-ok.com", msg(__LINE__) );


	// port
	const auto PORT_DEFAULT = 0, PORT_INVALID = -1;
	const bool PROTO_IS_HTTPS = true, PROTO_IS_HTTP = false;


	for (const auto &in : {""s, "0"s, "-80"s, "%38"s, std::to_string(std::numeric_limits<unsigned short>::max() + 1) })
		expected( normalize_port(in, PROTO_IS_HTTP), PORT_INVALID, msg(__LINE__) );

	expected( 
		normalize_port(std::to_string(std::numeric_limits<unsigned short>::max()), PROTO_IS_HTTP), 
		std::numeric_limits<unsigned short>::max(), 
		msg(__LINE__) 
	);

	expected( 
		normalize_port("80", PROTO_IS_HTTP), 
		PORT_DEFAULT,
		msg(__LINE__) 
	);

	expected( 
		normalize_port("80", PROTO_IS_HTTPS), 
		80,
		msg(__LINE__) 
	);

	expected(
		normalize_port("443", PROTO_IS_HTTPS), 
		PORT_DEFAULT,
		msg(__LINE__) 
	);

	expected(
		normalize_port("443", PROTO_IS_HTTP), 
		443,
		msg(__LINE__) 
	);

	// path
	for (const auto &in : {"", ".", "..", "./..", "//", "/", "/.", "//./../", "index.html", "/index.php"})
		expected( normalize_path(in), "", msg(__LINE__) );
	
	expected( normalize_path(" "), "%20", msg(__LINE__) );
	expected( normalize_path("x\0x"s), "x%00x", msg(__LINE__) );
	expected( normalize_path(""s), "", msg(__LINE__) );

	expected( normalize_path("x/y/z/default.asp"), "x/y/z", msg(__LINE__) );

	for (const auto &in : {"x/y/z", "/x/y/z", "x/y/z/", "x//y////z/", "%78//y////z/"})
		expected( normalize_path(in), "x/y/z", msg(__LINE__) );

	for (const auto &in : {"X/ /z", "/X/ /z", "X/ /z/", "X// ////z/", "%58// ////z/"})
		expected( normalize_path(in), "X/%20/z", msg(__LINE__) );

	expected( normalize_path("%2e%2e/hex/Sa/./sa/../%2e/index.html"), "hex/Sa/sa", msg(__LINE__) );
	expected( normalize_path("%2e%2e/hex/Sa/./sa/../%2e/index.html/"), "hex/Sa/sa", msg(__LINE__) );
	expected( normalize_path("..."), "...", msg(__LINE__) );
	expected( normalize_path("indeX.html"), "indeX.html", msg(__LINE__) );
	expected( normalize_path("indeX%2ehtml"), "indeX.html", msg(__LINE__) );
	expected( normalize_path("%2findeX%2ehtml"), "%2FindeX.html", msg(__LINE__) );

	expected( normalize_path("\x0f"), "%0F", msg(__LINE__) );
	expected( normalize_path("%0f"), "%0F", msg(__LINE__) );

	// query
	for (const auto &in : {"", "&", "&&&", "&=", "&=&", "=", "==", "==&", "&=&="})
		expected( normalize_query(in), "", msg(__LINE__) );

	for (const auto &in : {"x", "x=", "x=&", "x=&x=&x"})
		expected( normalize_query(in), "x", msg(__LINE__) );

	for (const auto &in : {"x=y", "x=z&x=y", "&x&x=&x=xyz&x=%79"})
		expected( normalize_query(in), "x=y", msg(__LINE__) );

	// spaces normalization
	for (const auto &in : {"x=y%20&z=k", "x=z&x=y &z=k", "&x&x=&x=xyz&x=y+&z=k"})
		expected( normalize_query(in), "x=y+&z=k", msg(__LINE__) );

	// sort parameter by key
	for (const auto &in : {"b=second&a=first&z=l%61st"})
		expected( normalize_query(in), "a=first&b=second&z=last", msg(__LINE__) );

#if 0
	/* tecnically it's possible make a kind of normalization but increase the complexity
	because we can't simply replace "&=" but we must check that prev_ch != '&' prev_ch && != '='
	(send a pull-request if you want) */
	for (const auto &in : {"x&=y"})
		expected( normalize_query(in), "x%26=y", msg(__LINE__) );
#endif

} catch (std::exception &ex) {
	cerr << 
	"-------------------------------------------------" << endl <<
	"--------------------TEST-FAILED------------------" << endl <<
	"-------------------------------------------------" << endl <<
		ex.what() << endl <<
	"-------------------------------------------------" << endl <<
	"------------------------END----------------------" << endl <<
	"-------------------------------------------------" << endl <<
	endl;

	return EXIT_FAILURE;
}