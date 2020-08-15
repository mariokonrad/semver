#ifndef SEMVER_RANGE_HPP
#define SEMVER_RANGE_HPP

#include <semver/semver.hpp>
#include <memory>
#include <optional>
#include <iostream>
#include <cassert>

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

---------------------------------------

	range-set  ::= '' | range ( logical-or range ) *
	logical-or ::= '||'
	range      ::= hyphen | simple+
	hyphen     ::= partial '-' partial
	simple     ::= partial | primitive | caret | tilde

	primitive  ::= op partial
	caret      ::= caret partial
	tilde      ::= tilde partial

	op         ::= '<' | '>' | '>=' | '<=' | '='

	partial    ::= xr ( '.' xr ( '.' xr qualifier ? )? )?
	xr         ::= 'x' | 'X' | '*' | nr
	nr         ::= '0' | ['1'-'9'] ( ['0'-'9'] ) *
	qualifier  ::= ( '-' pre )? ( '+' build )?
	pre        ::= parts
	build      ::= parts
	parts      ::= part ( '.' part ) *
	part       ::= nr | [-0-9A-Za-z]+
*/

namespace detail
{
class lexer
{
private:
	using char_type = std::string_view::value_type;

public:
	enum class token {
		partial,
		caret_partial,
		tilde_partial,
		op_partial,
		dash,
		logical_or,
		error,
		eof
	};

	static const char * name(token t) noexcept
	{
		switch (t) {
			case token::partial:
				return "partial";
			case token::caret_partial:
				return "caret_partial";
			case token::tilde_partial:
				return "tilde_partial";
			case token::op_partial:
				return "op_partial";
			case token::dash:
				return "dash";
			case token::logical_or:
				return "logical_or";
			case token::error:
				return "error";
			case token::eof:
				return "eof";
		}
		return "<unknown>";
	}

	lexer(std::string_view s)
		: last_(s.data() + s.size())
		, cursor_(s.data())
	{
	}

	token scan() noexcept
	{
		clear();
		while (!eof()) {
			start_ = cursor_;

			if (is_space()) {
				advance(1);
				continue;
			}

			if (is_dash()) {
				advance(1);
				store();
				return token::dash;
			}

			if (is_logical_or()) {
				advance(2);
				store();
				return token::logical_or;
			}

			if (is_caret())
				return lex_caret_partial();

			if (is_tilde())
				return lex_tilde_partial();

			if (is_op())
				return lex_op_partial();

			if (is_x() || is_star() || is_digit())
				return lex_partial();

			return token::error;
		}
		return token::eof;
	}

	std::string_view text() noexcept
	{
		return token_;
	}

private:
	const char_type * last_ = nullptr;
	const char_type * start_ = nullptr;
	const char_type * cursor_ = nullptr;
	const char_type * error_ = nullptr;
	std::string_view token_ = {};

	void clear() noexcept { token_ = {}; }

	void store()
	{
		assert(std::distance(start_, cursor_) >= 0);
		token_ = std::string_view(
			start_, static_cast<std::size_t>(std::distance(start_, cursor_)));
	}

	void error() noexcept { error_ = cursor_; }
	void advance(int n) noexcept { cursor_ += std::min(n, static_cast<int>(last_ - cursor_)); }

	bool eof() const noexcept { return (cursor_ == last_) || (*cursor_ == '\x00'); }
	bool eof(const char_type * p) const noexcept { return (p == last_) || (*p == '\x00'); }
	bool is_space() const noexcept { return ::isspace(*cursor_); }
	bool is_digit() const noexcept { return ::isdigit(*cursor_); }
	bool is_caret() const noexcept { return *cursor_ == '^'; }
	bool is_tilde() const noexcept { return *cursor_ == '~'; }
	bool is_star() const noexcept { return *cursor_ == '*'; }
	bool is_dash() const noexcept { return *cursor_ == '-'; }
	bool is_dot() const noexcept { return *cursor_ == '.'; }
	bool is_plus() const noexcept { return *cursor_ == '+'; }
	bool is_x() const noexcept { return *cursor_ == 'x' || *cursor_ == 'X'; }
	bool is_zero() const noexcept { return *cursor_ == '0'; }

	bool is_positive_digit() const noexcept
	{
		return !eof() && ((*cursor_ >= '1') && (*cursor_ <= '9'));
	}

	bool is_letter() const noexcept
	{
		return !eof()
			&& (((*cursor_ >= 'A') && (*cursor_ <= 'Z'))
				|| ((*cursor_ >= 'a') && (*cursor_ <= 'z')));
	}

	char_type peek(int n = 1) const noexcept
	{
		const char_type * p = cursor_ + n;
		return eof(p) ? '\0' : *p;
	}

	bool is_lt() const noexcept { return *cursor_ == '<' && peek() != '='; }
	bool is_le() const noexcept { return *cursor_ == '<' && peek() == '='; }
	bool is_gt() const noexcept { return *cursor_ == '>' && peek() != '='; }
	bool is_ge() const noexcept { return *cursor_ == '>' && peek() == '='; }
	bool is_eq() const noexcept { return *cursor_ == '='; }
	bool is_op() const noexcept { return is_lt() || is_le() || is_gt() || is_ge() || is_eq(); }
	bool is_logical_or() const noexcept { return (*cursor_ == '|') && (peek() == '|'); }

	token lex_caret_partial() noexcept
	{
		advance(1); // caret
		scan_partial();
		store();
		return (!error_) ? token::caret_partial : token::error;
	}

	token lex_tilde_partial() noexcept
	{
		advance(1); // tilde
		scan_partial();
		store();
		return (!error_) ? token::tilde_partial : token::error;
	}

	token lex_op_partial() noexcept
	{
		if (is_eq() || is_lt() || is_gt())
			advance(1);
		else
			advance(2);
		scan_partial();
		store();
		return (!error_) ? token::op_partial : token::error;
	}

	token lex_partial() noexcept
	{
		scan_partial();
		store();
		return (!error_) ? token::partial : token::error;
	}

	void scan_partial() noexcept
	{
		scan_partial_version();

		if (!is_dot())
			return;
		advance(1);

		scan_partial_version();

		if (!is_dot())
			return;
		advance(1);

		scan_partial_version();

		if (is_dash()) {
			advance(1);
			scan_dot_separated_identifier(); // prerelease
			if (is_plus()) {
				advance(1);
				scan_dot_separated_identifier(); // build
			}
			return;
		}
		if (is_plus()) {
			advance(1);
			scan_dot_separated_identifier(); // build
			return;
		}
	}

	void scan_dot_separated_identifier() noexcept
	{
		scan_identifier();
		while (is_dot()) {
			advance(1);
			scan_identifier();
		}
	}

	void scan_identifier() noexcept
	{
		if (!(is_letter() || is_digit() || is_dash())) {
			error();
			return;
		}
		while (is_letter() || is_digit() || is_dash())
			advance(1);
	}

	void scan_partial_version() noexcept
	{
		if (is_x()) {
			advance(1);
			return;
		}
		if (is_star()) {
			advance(1);
			return;
		}
		if (is_zero()) {
			advance(1);
			return;
		}
		if (is_positive_digit()) {
			advance(1);
			while (is_digit())
				advance(1);
			return;
		}
		error();
	}
};

enum class node_type { op_and, op_or, op_eq, op_lt, op_le, op_gt, op_ge };

class node
{
public:
	~node() = default;

	node(const node &) = delete;
	node & operator=(const node &) = delete;

	node(node &&) = default;
	node & operator=(node &&) = default;

	static node create_and(std::unique_ptr<node> left, std::unique_ptr<node> right)
	{
		return node(node_type::op_and, std::move(left), std::move(right));
	}

	static node create_or(std::unique_ptr<node> left, std::unique_ptr<node> right)
	{
		return node(node_type::op_or, std::move(left), std::move(right));
	}

	static node create_eq(const semver & s) { return {node_type::op_eq, s}; }
	static node create_lt(const semver & s) { return {node_type::op_lt, s}; }
	static node create_le(const semver & s) { return {node_type::op_le, s}; }
	static node create_gt(const semver & s) { return {node_type::op_gt, s}; }
	static node create_ge(const semver & s) { return {node_type::op_ge, s}; }

	bool eval(const semver &) const noexcept
	{
		// TODO: implementation
		return false;
	}

private:
	node_type type_;
	std::unique_ptr<node> left_;
	std::unique_ptr<node> right_;
	std::optional<semver> version_;

	node(node_type t, const semver & s)
		: type_(t)
		, version_(s)
	{
	}

	node(node_type t, std::unique_ptr<node> left, std::unique_ptr<node> right)
		: type_(t)
		, left_(std::move(left))
		, right_(std::move(right))
	{
	}
};
}

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
		, lex_(std::string_view(s.data(), s.size()))
	{
		parse_range_set();
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
	using lexer = detail::lexer;
	using token = lexer::token;

	std::string data_;
	bool good_ = false;

	detail::lexer lex_;
	detail::lexer::token token_ = detail::lexer::token::eof;
	detail::lexer::token next_ = detail::lexer::token::eof;
	std::string_view token_text_;
	std::string_view next_text_;

	std::unique_ptr<detail::node> ast_;

	void start() noexcept
	{
		token_ = lex_.scan();
		token_text_ = lex_.text();
		next_ = lex_.scan();
		next_text_ = lex_.text();
	}

	void advance() noexcept
	{
		token_ = next_;
		token_text_ = next_text_;
		next_ = lex_.scan();
		next_text_ = lex_.text();
	}

	std::string_view token_text() noexcept { return token_text_; }

	void error() noexcept { good_ = false; }

	bool is_eof(token t) const noexcept { return t == token::eof; }
	bool is_error(token t) const noexcept { return t == token::error; }
	bool is_caret(token t) const noexcept { return t == token::caret_partial; }
	bool is_tilde(token t) const noexcept { return t == token::tilde_partial; }
	bool is_op(token t) const noexcept { return t == token::op_partial; }
	bool is_logical_or(token t) const noexcept { return t == token::logical_or; }
	bool is_partial(token t) const noexcept { return t == token::partial; }
	bool is_dash(token t) const noexcept { return t == token::dash; }

	void parse_range_set() noexcept
	{
		start();

		if (is_eof(token_)) {
			good_ = true;
			return;
		}

		parse_range();
		while (is_logical_or(token_)) {
			advance(); // logial-or
			std::cout << "### logical-or\n";
			parse_range();
		}

		good_ = !is_error(token_);
	}

	void parse_range() noexcept
	{
		if (is_partial(token_) && is_dash(next_)) {
			auto sv = token_text();
			advance(); // partial
			advance(); // dash
			if (is_partial(token_)) {
				std::cout << "### hyphen ["<< sv <<"] - ["<<token_text()<<"]\n";
				advance();
				return;
			}
			error();
			return;
		}

		while (!is_eof(token_)) {
			if (is_caret(token_)) {
				std::cout << "### caret partial ["<<token_text()<<"]\n";
				advance();
				continue;
			}
			if (is_tilde(token_)) {
				std::cout << "### tilde partial ["<<token_text()<<"]\n";
				advance();
				continue;
			}
			if (is_op(token_)) {
				std::cout << "### op partial ["<<token_text()<<"]\n";
				advance();
				continue;
			}
			if (is_partial(token_)) {
				std::cout << "### partial ["<<token_text()<<"]\n";
				advance();
				continue;
			}

			error();
			return;
		}
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

