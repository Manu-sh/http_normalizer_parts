#pragma once

#include <string>
#include <string_view>
#include <sstream>
#include <algorithm>
#include <functional>
#include <vector>
#include <iterator>
#include <type_traits>

#include <iostream>
using std::cout, std::endl;

namespace http_parts::utils {

	// some alias
	template <typename T>
	using IteratorCategory = typename std::iterator_traits<T>::iterator_category;

	template <typename T>
	using IteratorValue = typename std::iterator_traits<T>::value_type;

	template <typename BDIter>
	static BDIter find_end_if(
		const BDIter beg, const BDIter end, 
		const std::function<bool(const IteratorValue<BDIter> &)> &predicate, 
		std::bidirectional_iterator_tag) 
	{

		for (auto e = end - 1; e != beg; --e)
			if (predicate(*e)) return e;

		return end;
	}
	
	template<typename Iter>
	static Iter find_end_if(
		const Iter first, const Iter last,
		const std::function<bool(const IteratorValue<Iter> &)> &predicate) 
	{
		return find_end_if(first, last, predicate, IteratorCategory<Iter>{});
	}

	// if there is nothing to split return an empty vector
	static std::vector<std::string> split(const std::string_view &s, const std::string_view &set) {

		std::vector<std::string> vct(0);
		std::string_view::const_iterator it[2] {s.cbegin(), s.cend()};

		// TODO capturing set into static lambda is a bug?
		const auto &e_contained     = [&set](const char e) { return set.find(e) == std::string_view::npos; };
		const auto &e_not_contained = [&set](const char e) { return set.find(e) != std::string_view::npos; };

		while (1) {

			/* if there isn't no occurrence of the delimiters listed into set return
			otherwise it[0] point to the first delimiter occurrence */
			if ((it[0] = std::find_if(it[0], s.cend(), e_contained)) == s.cend())
				return vct;

			/* from the first delimiter occurrence it[0] stop at the next delimiter occurrence
			and assign it to it[1] */
			if ((it[1] = std::find_if(it[0], s.cend(), e_not_contained)) == s.cend()) {
				vct.emplace_back(it[0], s.cend());
				return vct;
			}

			/* create a string from two iterators it[0] to it[1] (the end) since are
			iterators the interval is (start, end] and the delimiter is cutted off	*/
			vct.emplace_back(it[0], it[1]);

			/* move the iterator */
			it[0] = it[1];
		}

		return vct;
	}

	static std::string join(const std::vector<std::string> &vct, const std::string_view &delimit) {

		std::ostringstream os;
		if (vct.empty()) return "";

		// find the first non-empty element from the end || take the first
		auto last_non_empty = find_end_if(vct.cbegin(), vct.cend(), [] (const auto &s) { return !s.empty(); });
		if (last_non_empty == vct.cend()) 
			last_non_empty = vct.cbegin();

		for (auto beg = vct.cbegin(); beg != last_non_empty; ++beg)
			if (!beg->empty()) os << *beg << delimit;

		// last_non_empty is a valid position here
		return (os << *last_non_empty), os.str();
	}

	// taken from: https://bits.mdminhazulhaque.io/cpp/find-and-replace-all-occurrences-in-cpp-string.html
	static std::string find_and_replace(const std::string_view &src, const std::string_view &find, const std::string_view &replace) {

		std::string source{src};
		for (std::string::size_type i = 0; (i = source.find(find, i)) != std::string::npos;) {
			source.replace(i, find.length(), replace);
			i += replace.length();
		}

		return source;
	}

}