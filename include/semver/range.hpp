#ifndef SEMVER_RANGE_HPP
#define SEMVER_RANGE_HPP

#include "detail/range_node.hpp"
#include "detail/range_parser.hpp"
#include <semver/semver.hpp>
#include <sstream>
#include <memory>
#include <vector>
#include <cassert>

namespace semver
{
inline namespace v1
{
class range final
{
public:
	using number_type = detail::range_parser::number_type;

	~range() = default;

	range(const range &) = default;
	range & operator=(const range &) = default;

	range(range &&) = default;
	range & operator=(range &&) = default;

	range(const std::string & s)
	{
		auto p = detail::range_parser(s);
		good_ = p.ok();
		if (good_)
			nodes_ = p.ast();
	}

	bool ok() const noexcept { return good_; }
	explicit operator bool() const noexcept { return ok(); }

	semver max() const noexcept
	{
		// TODO: idea for algorithm:
		//
		//   1. collect all min/max of 'or' nodes -> v_min, v_max
		//
		//   2. collect min/max from all 'and' nodes using (min|max)_satisfying
		//      for all 'and':
		//      - min = min_satisfying({local min})
		//      - max = max_satisfying({local max})
		//
		//   3. determine min/max using (min|max)_satisfying from v_min/v_max
		//

		// TODO: implementation
		return {""};
	}

	semver min() const noexcept
	{
		// TODO: implementation
		return {""};
	}

	bool satisfies(const semver & v) const noexcept
	{
		if (nodes_.empty())
			return false;

		// all nodes in AST are implicit `or`
		for (const auto & n : nodes_)
			if (n->eval(v))
				return true;

		return false;
	}

	semver max_satisfying(const std::vector<semver> & versions) const noexcept
	{
		return satisfies_if(begin(versions), end(versions), std::greater<> {});
	}

	semver min_satisfying(const std::vector<semver> & versions) const noexcept
	{
		return satisfies_if(begin(versions), end(versions), std::less<> {});
	}

	bool outside(const semver & v) const noexcept { return !satisfies(v); }

private:
	bool good_ = false;
	std::vector<std::unique_ptr<detail::node>> nodes_;

	template <typename Iterator, typename Predicate>
	typename Iterator::value_type satisfies_if(
		Iterator first, Iterator last, Predicate pred) const noexcept
	{
		if (first == last)
			return {};
		bool never_checked = true;
		typename Iterator::value_type result = {};
		for (; first != last; ++first) {
			if (satisfies(*first)) {
				if (never_checked) {
					never_checked = false;
					result = *first;
				} else if (pred(*first, result)) {
					result = *first;
				}
			}
		}
		return result;
	}
	friend bool operator==(const range & r1, const range & r2) noexcept
	{
		if (r1.nodes_.size() != r2.nodes_.size())
			return false;

		return std::equal(begin(r1.nodes_), end(r1.nodes_), begin(r2.nodes_),
			[](const auto & a, const auto & b) { return *a == *b; });
	}

	friend bool operator!=(const range & r1, const range & r2) noexcept { return !(r1 == r2); }

	friend std::string to_string(const range &);
};

inline std::string to_string(const range & r)
{
	std::ostringstream os;

	bool first = true;
	for (const auto & n : r.nodes_) {
		if (first) {
			first = false;
		} else {
			os << " || ";
		}

		os << *n;
	}

	return os.str();
}

inline std::ostream & operator<<(std::ostream & os, const range & r)
{
	return os << to_string(r);
}

inline bool intersect(const range &, const range &) noexcept
{
	// TODO: implementation
	return false;
}
}
}

#endif

