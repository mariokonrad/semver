#ifndef SEMVER_RANGE_HPP
#define SEMVER_RANGE_HPP

#include <semver/semver.hpp>

namespace semver
{
inline namespace v1
{

/*
https://docs.npmjs.com/misc/semver

BNF:

	range-set  ::= range ( logical-or range ) *
	logical-or ::= ( ' ' ) * '||' ( ' ' ) *
	range      ::= hyphen | simple ( ' ' simple ) * | ''
	hyphen     ::= partial ' - ' partial
	simple     ::= primitive | partial | tilde | caret
	primitive  ::= ( '<' | '>' | '>=' | '<=' | '=' ) partial
	partial    ::= xr ( '.' xr ( '.' xr qualifier ? )? )?
	xr         ::= 'x' | 'X' | '*' | nr
	nr         ::= '0' | ['1'-'9'] ( ['0'-'9'] ) *
	tilde      ::= '~' partial
	caret      ::= '^' partial
	qualifier  ::= ( '-' pre )? ( '+' build )?
	pre        ::= parts
	build      ::= parts
	parts      ::= part ( '.' part ) *
	part       ::= nr | [-0-9A-Za-z]+


cheat sheet: https://devhints.io/semver

*/

class range
{
private:
	using char_type = std::string::value_type;

public:
	using number_type = unsigned long;

	~range() = default;

	range(const range &) = default;
	range & operator=(const range &) = default;

	range(range &&) = default;
	range & operator=(range &&) = default;

	range(const std::string & s)
		: data_(s)
	{
		last_ = data_.data() + data_.size();
		cursor_ = data_.data();

		parse_range_set();
		good_ = (cursor_ == last_) && !error_;
	}

	bool ok() const noexcept { return good_; }
	explicit operator bool() const noexcept { return ok(); }

	semver max() const noexcept
	{
		// TODO: implementation
		return {""};
	}

	semver min() const noexcept
	{
		// TODO: implementation
		return {""};
	}

	bool satisfies(const semver &) const noexcept
	{
		// TODO: implementation
		return false;
	}

	semver max_satisfying(const std::vector<semver> &) const noexcept
	{
		// TODO: implementation
		return {""};
	}

	semver min_satisfying(const std::vector<semver> &) const noexcept
	{
		// TODO: implementation
		return {""};
	}

	bool outside(const semver &) const noexcept
	{
		// TODO: implementation
		return false;
	}

private:
	const char_type * last_ = {};
	const char_type * start_ = {};
	const char_type * cursor_ = {};
	const char_type * error_ = nullptr;

	std::string data_;
	bool good_ = false;

	static std::string_view token(const char_type * start, const char_type * end) noexcept
	{
		return std::string_view {start, static_cast<std::string_view::size_type>(end - start)};
	}

	void advance(int n) noexcept { cursor_ += std::min(n, static_cast<int>(last_ - cursor_)); }

	void error() noexcept { error_ = cursor_; }

	void parse_range_set() noexcept
	{
		parse_range();
		while (is_pipe(cursor_)) {
			parse_logical_or();
			parse_range();
		}
	}

	void parse_range() noexcept
	{
		if (is_eof(cursor_))
			return;

		// TODO: implementation
	}

	void parse_pipe() noexcept
	{
		if (!is_pipe(cursor_)) {
			error();
			return;
		}
		advance(1);
	}

	void parse_logical_or() noexcept
	{
		parse_pipe();
		parse_pipe();
	}

	// low level primitives, must check for eof

	bool is_eof(const char_type * p) const noexcept { return (p >= last_) || (*p == '\x00'); }

	bool is_pipe(const char_type * p) const noexcept { return !is_eof(p) && (*p == '|'); }

	bool is_caret(const char_type * p) const noexcept { return !is_eof(p) && (*p == '^'); }

	bool is_tilde(const char_type * p) const noexcept { return !is_eof(p) && (*p == '~'); }
};

inline bool intersect(const range &, const range &) noexcept
{
	// TODO: implementation
	return false;
}

inline bool operator==(const range &, const range &) noexcept
{
	// TODO: implementation
	return false;
}

inline bool operator!=(const range & r1, const range & r2) noexcept
{
	return !(r1 == r2);
}
}
}

#endif

