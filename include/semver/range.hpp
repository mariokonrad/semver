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

static std::string trim(std::string s)
{
	const auto b = s.find_first_not_of(" ");
	const auto e = s.find_last_not_of(" ");

	return (b != std::string::npos) ? s.substr(b, e - b + 1) : "";
}

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
		: data_(trim(s))
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

	number_type major_ = {};
	number_type minor_ = {};
	number_type patch_ = {};
	std::string_view prerelease_ = {};
	std::string_view build_ = {};

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
		if (is_eof(cursor_))
			return;

		parse_range();
		while (is_pipe(cursor_)) {
			skip_space();
			parse_logical_or();
			skip_space();
			parse_range();
		}
	}

	void skip_space() noexcept
	{
		while (is_space(cursor_))
			advance(1);
	}

	void parse_range() noexcept
	{
		// TODO: implementation: hyphen: partial ' '  '-'  ' ' partial
		// TODO: implementation: simple (' ' simple)*
	}

	void parse_logical_or() noexcept
	{
		if (is_pipe(cursor_) && is_pipe(cursor_ + 1)) {
			advance(2);
			return;
		}
		error();
	}

	void parse_simple() noexcept
	{
		if (is_tilde(cursor_))
			advance(1);

		if (is_caret(cursor_))
			advance(1);

		if (is_lt(cursor_))
			advance(1);
		if (is_le(cursor_))
			advance(1);
		if (is_gt(cursor_))
			advance(1);
		if (is_ge(cursor_))
			advance(1);
		if (is_eq(cursor_))
			advance(1);

		parse_partial();
	}

	void parse_dot() noexcept
	{
		if (is_dot(cursor_)) {
			advance(1);
			return;
		}
		error();
	}

	void parse_build() noexcept
	{
		start_ = cursor_;
		parse_dot_separated_identifier();
		build_ = token(start_, cursor_);
	}

	void parse_pre_release() noexcept
	{
		start_ = cursor_;
		parse_dot_separated_identifier();
		prerelease_ = token(start_, cursor_);
	}

	void parse_dot_separated_identifier() noexcept
	{
		parse_identifier();
		while (is_dot(cursor_)) {
			parse_dot();
			parse_identifier();
		}
	}

	void parse_identifier() noexcept
	{
		if (!(is_letter(cursor_) || is_digit(cursor_) || is_dash(cursor_))) {
			error();
			return;
		}
		while (is_letter(cursor_) || is_digit(cursor_) || is_dash(cursor_))
			advance(1);
	}

	void parse_partial() noexcept
	{
		parse_partial_version();
		if (!is_dot(cursor_))
			return;
		parse_dot();
		parse_partial_version();
		if (!is_dot(cursor_))
			return;
		parse_dot();
		parse_partial_version();

		if (is_dash(cursor_)) {
			advance(1);
			parse_pre_release();
			if (is_plus(cursor_)) {
				advance(1);
				parse_build();
			}
			return;
		}
		if (is_plus(cursor_)) {
			advance(1);
			parse_build();
			return;
		}
	}

	void parse_partial_version() noexcept
	{
		if (is_x(cursor_)) {
			advance(1);
			return;
		}
		if (is_star(cursor_)) {
			advance(1);
			return;
		}
		if (is_zero(cursor_)) {
			advance(1);
			return;
		}
		if (is_positive_digit(cursor_)) {
			parse_positive_digit();
			if (is_digit(cursor_))
				parse_digits();
			return;
		}
		error();
	}

	void parse_positive_digit() noexcept
	{
		if (is_positive_digit(cursor_)) {
			advance(1);
			return;
		}
		error();
	}

	void parse_digits() noexcept
	{
		if (!is_digit(cursor_)) {
			error();
			return;
		}
		while (is_digit(cursor_))
			advance(1);
	}

	// low level primitives, must check for eof

	bool peek(char_type c, const char_type * p) const noexcept
	{
		return !is_eof(p) && (*p == c);
	}

	bool is_eof(const char_type * p) const noexcept { return (p >= last_) || (*p == '\x00'); }
	bool is_pipe(const char_type * p) const noexcept { return peek('|', p); }
	bool is_caret(const char_type * p) const noexcept { return peek('^', p); }
	bool is_tilde(const char_type * p) const noexcept { return peek('~', p); }
	bool is_star(const char_type * p) const noexcept { return peek('*', p); }
	bool is_space(const char_type * p) const noexcept { return peek(' ', p); }
	bool is_eq(const char_type * p) const noexcept { return peek('=', p); }
	bool is_lt(const char_type * p) const noexcept { return peek('<', p) && !peek('=', p + 1); }
	bool is_le(const char_type * p) const noexcept { return peek('<', p) && peek('=', p + 1); }
	bool is_gt(const char_type * p) const noexcept { return peek('>', p) && !peek('=', p + 1); }
	bool is_ge(const char_type * p) const noexcept { return peek('>', p) && peek('=', p + 1); }
	bool is_dot(const char_type * p) const noexcept { return peek('.', p); }
	bool is_plus(const char_type * p) const noexcept { return peek('+', p); }
	bool is_dash(const char_type * p) const noexcept { return peek('-', p); }
	bool is_zero(const char_type * p) const noexcept { return peek('0', p); }

	bool is_x(const char_type * p) const noexcept
	{
		return !is_eof(p) && ((*p == 'x') || (*p == 'X'));
	}

	bool is_positive_digit(const char_type * p) const noexcept
	{
		return !is_eof(p) && ((*p >= '1') && (*p <= '9'));
	}

	bool is_digit(const char_type * p) const noexcept
	{
		return !is_eof(p) && ((*p >= '0') && (*p <= '9'));
	}

	bool is_letter(const char_type * p) const noexcept
	{
		return !is_eof(p) && (((*p >= 'A') && (*p <= 'Z')) || ((*p >= 'a') && (*p <= 'z')));
	}
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

