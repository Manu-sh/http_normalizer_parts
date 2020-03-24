#pragma once
#include "utils.hpp"

#include <cctype>
#include <string>
#include <iterator> // std::size
#include <algorithm>
#include <unordered_map>

#include <string_view>
#include <sstream>
#include <functional>

/*
	A percent-encoded octet is encoded as a character
	triplet, consisting of the percent character "%" followed by the two
	hexadecimal digits representing that octet's numeric value.  For
	example, "%20" is the percent-encoding for the binary octet
	"00100000" (ABNF: %x20), which in US-ASCII corresponds to the space
	character (SP).


	The uppercase hexadecimal digits 'A' through 'F' are equivalent to
	the lowercase digits 'a' through 'f', respectively.  If two URIs
	differ only in the case of hexadecimal digits used in percent-encoded
	octets, they are equivalent.  For consistency, URI producers and
	normalizers should use uppercase hexadecimal digits for all percent-
	encodings.
*/

namespace http_parts::pct {

	// TODO EOF se non è presente
	// std::string altrimenti l'hash è l'indirizzo
	static int pdec_ch(const char *single_enc_char) {

		static std::unordered_map<std::string, unsigned char> dec_tab {
			{ "%00",   0 }, { "%01",   1 }, { "%02",   2 }, { "%03",   3 }, { "%04",   4 }, { "%05",   5 },
			{ "%06",   6 }, { "%07",   7 }, { "%08",   8 }, { "%09",   9 }, { "%0A",  10 }, { "%0B",  11 },
			{ "%0C",  12 }, { "%0D",  13 }, { "%0E",  14 }, { "%0F",  15 }, { "%10",  16 }, { "%11",  17 },
			{ "%12",  18 }, { "%13",  19 }, { "%14",  20 }, { "%15",  21 }, { "%16",  22 }, { "%17",  23 },
			{ "%18",  24 }, { "%19",  25 }, { "%1A",  26 }, { "%1B",  27 }, { "%1C",  28 }, { "%1D",  29 },
			{ "%1E",  30 }, { "%1F",  31 }, { "%20",  32 }, { "%21",  33 }, { "%22",  34 }, { "%23",  35 },
			{ "%24",  36 }, { "%25",  37 }, { "%26",  38 }, { "%27",  39 }, { "%28",  40 }, { "%29",  41 },
			{ "%2A",  42 }, { "%2B",  43 }, { "%2C",  44 }, { "%2D",  45 }, { "%2E",  46 }, { "%2F",  47 },
			{ "%30",  48 }, { "%31",  49 }, { "%32",  50 }, { "%33",  51 }, { "%34",  52 }, { "%35",  53 },
			{ "%36",  54 }, { "%37",  55 }, { "%38",  56 }, { "%39",  57 }, { "%3A",  58 }, { "%3B",  59 },
			{ "%3C",  60 }, { "%3D",  61 }, { "%3E",  62 }, { "%3F",  63 }, { "%40",  64 }, { "%41",  65 },
			{ "%42",  66 }, { "%43",  67 }, { "%44",  68 }, { "%45",  69 }, { "%46",  70 }, { "%47",  71 },
			{ "%48",  72 }, { "%49",  73 }, { "%4A",  74 }, { "%4B",  75 }, { "%4C",  76 }, { "%4D",  77 },
			{ "%4E",  78 }, { "%4F",  79 }, { "%50",  80 }, { "%51",  81 }, { "%52",  82 }, { "%53",  83 },
			{ "%54",  84 }, { "%55",  85 }, { "%56",  86 }, { "%57",  87 }, { "%58",  88 }, { "%59",  89 },
			{ "%5A",  90 }, { "%5B",  91 }, { "%5C",  92 }, { "%5D",  93 }, { "%5E",  94 }, { "%5F",  95 },
			{ "%60",  96 }, { "%61",  97 }, { "%62",  98 }, { "%63",  99 }, { "%64", 100 }, { "%65", 101 },
			{ "%66", 102 }, { "%67", 103 }, { "%68", 104 }, { "%69", 105 }, { "%6A", 106 }, { "%6B", 107 },
			{ "%6C", 108 }, { "%6D", 109 }, { "%6E", 110 }, { "%6F", 111 }, { "%70", 112 }, { "%71", 113 },
			{ "%72", 114 }, { "%73", 115 }, { "%74", 116 }, { "%75", 117 }, { "%76", 118 }, { "%77", 119 },
			{ "%78", 120 }, { "%79", 121 }, { "%7A", 122 }, { "%7B", 123 }, { "%7C", 124 }, { "%7D", 125 },
			{ "%7E", 126 }, { "%7F", 127 }, { "%80", 128 }, { "%81", 129 }, { "%82", 130 }, { "%83", 131 },
			{ "%84", 132 }, { "%85", 133 }, { "%86", 134 }, { "%87", 135 }, { "%88", 136 }, { "%89", 137 },
			{ "%8A", 138 }, { "%8B", 139 }, { "%8C", 140 }, { "%8D", 141 }, { "%8E", 142 }, { "%8F", 143 },
			{ "%90", 144 }, { "%91", 145 }, { "%92", 146 }, { "%93", 147 }, { "%94", 148 }, { "%95", 149 },
			{ "%96", 150 }, { "%97", 151 }, { "%98", 152 }, { "%99", 153 }, { "%9A", 154 }, { "%9B", 155 },
			{ "%9C", 156 }, { "%9D", 157 }, { "%9E", 158 }, { "%9F", 159 }, { "%A0", 160 }, { "%A1", 161 },
			{ "%A2", 162 }, { "%A3", 163 }, { "%A4", 164 }, { "%A5", 165 }, { "%A6", 166 }, { "%A7", 167 },
			{ "%A8", 168 }, { "%A9", 169 }, { "%AA", 170 }, { "%AB", 171 }, { "%AC", 172 }, { "%AD", 173 },
			{ "%AE", 174 }, { "%AF", 175 }, { "%B0", 176 }, { "%B1", 177 }, { "%B2", 178 }, { "%B3", 179 },
			{ "%B4", 180 }, { "%B5", 181 }, { "%B6", 182 }, { "%B7", 183 }, { "%B8", 184 }, { "%B9", 185 },
			{ "%BA", 186 }, { "%BB", 187 }, { "%BC", 188 }, { "%BD", 189 }, { "%BE", 190 }, { "%BF", 191 },
			{ "%C0", 192 }, { "%C1", 193 }, { "%C2", 194 }, { "%C3", 195 }, { "%C4", 196 }, { "%C5", 197 },
			{ "%C6", 198 }, { "%C7", 199 }, { "%C8", 200 }, { "%C9", 201 }, { "%CA", 202 }, { "%CB", 203 },
			{ "%CC", 204 }, { "%CD", 205 }, { "%CE", 206 }, { "%CF", 207 }, { "%D0", 208 }, { "%D1", 209 },
			{ "%D2", 210 }, { "%D3", 211 }, { "%D4", 212 }, { "%D5", 213 }, { "%D6", 214 }, { "%D7", 215 },
			{ "%D8", 216 }, { "%D9", 217 }, { "%DA", 218 }, { "%DB", 219 }, { "%DC", 220 }, { "%DD", 221 },
			{ "%DE", 222 }, { "%DF", 223 }, { "%E0", 224 }, { "%E1", 225 }, { "%E2", 226 }, { "%E3", 227 },
			{ "%E4", 228 }, { "%E5", 229 }, { "%E6", 230 }, { "%E7", 231 }, { "%E8", 232 }, { "%E9", 233 },
			{ "%EA", 234 }, { "%EB", 235 }, { "%EC", 236 }, { "%ED", 237 }, { "%EE", 238 }, { "%EF", 239 },
			{ "%F0", 240 }, { "%F1", 241 }, { "%F2", 242 }, { "%F3", 243 }, { "%F4", 244 }, { "%F5", 245 },
			{ "%F6", 246 }, { "%F7", 247 }, { "%F8", 248 }, { "%F9", 249 }, { "%FA", 250 }, { "%FB", 251 },
			{ "%FC", 252 }, { "%FD", 253 }, { "%FE", 254 }, { "%FF", 255 }
		};

		std::string key{single_enc_char};
		std::transform(key.begin(), key.end(), key.begin(), ::toupper);
		return dec_tab[key] == 0 && key != "%00" ? -1 : dec_tab[key];

	}

	static const char * pec_ch(unsigned char i) {

		const static char *const enc_tab[] = {
			"%00", "%01", "%02", "%03", "%04", "%05", "%06", "%07", "%08", "%09", "%0A", "%0B", "%0C", "%0D", "%0E", "%0F",
			"%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17", "%18", "%19", "%1A", "%1B", "%1C", "%1D", "%1E", "%1F", 
			"%20", "%21", "%22", "%23", "%24", "%25", "%26", "%27", "%28", "%29", "%2A", "%2B", "%2C", "%2D", "%2E", "%2F", 
			"%30", "%31", "%32", "%33", "%34", "%35", "%36", "%37", "%38", "%39", "%3A", "%3B", "%3C", "%3D", "%3E", "%3F", 
			"%40", "%41", "%42", "%43", "%44", "%45", "%46", "%47", "%48", "%49", "%4A", "%4B", "%4C", "%4D", "%4E", "%4F", 
			"%50", "%51", "%52", "%53", "%54", "%55", "%56", "%57", "%58", "%59", "%5A", "%5B", "%5C", "%5D", "%5E", "%5F", 
			"%60", "%61", "%62", "%63", "%64", "%65", "%66", "%67", "%68", "%69", "%6A", "%6B", "%6C", "%6D", "%6E", "%6F", 
			"%70", "%71", "%72", "%73", "%74", "%75", "%76", "%77", "%78", "%79", "%7A", "%7B", "%7C", "%7D", "%7E", "%7F", 
			"%80", "%81", "%82", "%83", "%84", "%85", "%86", "%87", "%88", "%89", "%8A", "%8B", "%8C", "%8D", "%8E", "%8F", 
			"%90", "%91", "%92", "%93", "%94", "%95", "%96", "%97", "%98", "%99", "%9A", "%9B", "%9C", "%9D", "%9E", "%9F", 
			"%A0", "%A1", "%A2", "%A3", "%A4", "%A5", "%A6", "%A7", "%A8", "%A9", "%AA", "%AB", "%AC", "%AD", "%AE", "%AF", 
			"%B0", "%B1", "%B2", "%B3", "%B4", "%B5", "%B6", "%B7", "%B8", "%B9", "%BA", "%BB", "%BC", "%BD", "%BE", "%BF", 
			"%C0", "%C1", "%C2", "%C3", "%C4", "%C5", "%C6", "%C7", "%C8", "%C9", "%CA", "%CB", "%CC", "%CD", "%CE", "%CF", 
			"%D0", "%D1", "%D2", "%D3", "%D4", "%D5", "%D6", "%D7", "%D8", "%D9", "%DA", "%DB", "%DC", "%DD", "%DE", "%DF", 
			"%E0", "%E1", "%E2", "%E3", "%E4", "%E5", "%E6", "%E7", "%E8", "%E9", "%EA", "%EB", "%EC", "%ED", "%EE", "%EF", 
			"%F0", "%F1", "%F2", "%F3", "%F4", "%F5", "%F6", "%F7", "%F8", "%F9", "%FA", "%FB", "%FC", "%FD", "%FE", "%FF"
		};

		return enc_tab[i];
	}

	static std::string pec_encode(const void *mem, int size) {

		std::string ret; 
		ret.reserve( (std::size("%00") - 1) * size + 1);

		for (int i = 0; i < size; i++) {
			std::string pc(pec_ch(((unsigned char *)mem)[i]), 3);
			ret += pc;
		}

		return ret;
	}

	template <typename T>
	static std::string pec_encode(const std::basic_string<T> &bs) {
		return pec_encode(bs.c_str(), sizeof(T) * bs.length());
	}

	static std::string pec_decode(const std::string &s) {

		std::stringstream ret;
		char pct[4] = {'\0', '\0', '\0', '\0'};

		for (size_t i = 0; i < s.length(); i++) {

			if (s[i] != '%') {
				ret.put(s[i]);
				continue;
			}

			int ch = s[i];
			pct[0] = s[i]; pct[1] = s[i+1]; pct[2] = s.at(i+2);

			if ((ch = pdec_ch(pct)) == -1) {
				ret.put(s[i]);
				continue;
			}

			ret.put(ch);
			i += 2;
		}

		return ret.str();
	}

	// ch_transform perform a transformation on each character into his non percent encoded form ex. if ch_tranform is tolower 
	// %6f -> (O -> ch_transform(O) -> o) -> %3F
	// input -> pcdecode -> ch_transfrom -> if is_reserved -> encode

	static std::string pec_normalize(const std::string_view &in, 
				const std::function<bool(char)> &is_reserved,
				const std::function<char(char)> &ch_transform = [] (char ch) { return ch; },
				bool encode_reserved = true) {

			const auto &pct_normalize = [&is_reserved, &ch_transform](char buf[3]) {
				int tmp;
				if ((tmp = pdec_ch(buf)) == -1)
					return 0; // error

				// this also normalize pct 2 upper-case if is reserved es. %3f -> %3F
				tmp = ch_transform((char)tmp);
				return is_reserved((char)tmp) ? ((buf[1] = toupper(buf[1])), (buf[2] = toupper(buf[2])), 3) : ((buf[0] = (char)tmp), 1);
			};

			std::string_view::iterator it[2] {in.cbegin(), in.cend()};
			std::ostringstream ret;

			while (it[0] != it[1]) {

				// es. tolower opt
				char ch = *it[0];

				if (ch == '%' && it[0] + 2 < it[1]) {
					char buf[3] = { it[0][0], it[0][1], it[0][2] };
					if (int len = pct_normalize(buf); len > 0) {
						ret.write(buf, len);
						it[0] += 3;
						continue;
					}
				}

				ch = ch_transform(ch);
				// if reserved && flag enc_reserved
				if (is_reserved(ch) && encode_reserved)
					ret.write(pec_ch(ch), 3);
				else 
					ret.put(ch);

				++it[0];
			}

		return ret.str();
	}

}