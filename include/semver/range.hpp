#ifndef SEMVER_RANGE_HPP
#define SEMVER_RANGE_HPP

#include <semver/semver.hpp>
#include <memory>
#include <optional>
#include <deque>
#include <iostream>
#include <cassert>

namespace semver
{
inline namespace v1
{

/*
https://github.com/npm/node-semver#versions
https://semver.npmjs.com/

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
class lexer final
{
public:
	using string_type = std::string;

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

	struct parts {
		string_type token = {};

		string_type op = {};
		string_type major = {};
		string_type minor = {};
		string_type patch = {};
		string_type prerelease = {};
		string_type build = {};
	};

	lexer(const std::string & s)
		: data_(s)
	{
		cursor_ = data_.data();
		last_ = data_.data() + data_.size();
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

	const string_type & data() const noexcept { return data_; }

	const parts & text() noexcept { return parts_; }

private:
	using char_type = string_type::value_type;

	const char_type * cursor_ = nullptr;
	const char_type * last_ = nullptr;
	const char_type * start_ = nullptr;
	const char_type * error_ = nullptr;

	parts parts_;
	std::string data_;

	void patch(char c) noexcept
	{
		assert(cursor_ >= data_.data());
		assert(cursor_ < data_.data() + data_.size());
		char_type * p = const_cast<char_type *>(cursor_); // we know what we are doing
		*p = c;
	}

	void clear() noexcept { parts_ = {}; }

	void store() noexcept { store(parts_.token, start_); }

	void store(string_type & s, const char_type * start) noexcept
	{
		const auto d = std::distance(start, cursor_);
		assert(d >= 0);
		s = string_type(start, static_cast<std::size_t>(d));
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
		const char_type * p = cursor_;
		if (is_eq() || is_lt() || is_gt())
			advance(1);
		else
			advance(2);
		store(parts_.op, p);

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

	void scan_major() noexcept
	{
		const char_type * p = cursor_;
		scan_partial_version();
		store(parts_.major, p);
	}

	void scan_minor() noexcept
	{
		const char_type * p = cursor_;
		scan_partial_version();
		store(parts_.minor, p);
	}

	void scan_patch() noexcept
	{
		const char_type * p = cursor_;
		scan_partial_version();
		store(parts_.patch, p);
	}

	void scan_prerelease() noexcept
	{
		const char_type * p = cursor_;
		scan_dot_separated_identifier();
		store(parts_.prerelease, p);
	}

	void scan_build() noexcept
	{
		const char_type * p = cursor_;
		scan_dot_separated_identifier();
		store(parts_.build, p);
	}

	void scan_partial() noexcept
	{
		scan_major();

		if (!is_dot())
			return;
		advance(1);

		scan_minor();

		if (!is_dot())
			return;
		advance(1);

		scan_patch();

		if (is_dash()) {
			advance(1);
			scan_prerelease();
			if (is_plus()) {
				advance(1);
				scan_build();
			}
			return;
		}
		if (is_plus()) {
			advance(1);
			scan_build();
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
			patch('*'); // normalize wildcards to '*'
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

inline semver lower_bound(const lexer::parts & p) noexcept
{
	// TODO: to be optimized, re-parse semver? meh...

	if (p.major.empty() || p.major == "*")
		return semver::min();

	if (p.minor.empty() || p.minor == "*")
		return semver(p.major + ".0.0");

	if (p.patch.empty() || p.patch == "*")
		return semver(p.major + "." + p.minor + ".0");

	return semver(p.token);
}

inline semver upper_bound(const lexer::parts & p) noexcept
{
	// TODO: to be optimized, re-parse semver? meh...

	if (p.major.empty() || p.major == "*")
		return semver::max();

	if (p.minor.empty() || p.minor == "*")
		return semver(std::to_string(std::stoul(p.major) + 1u) + ".0.0-0");

	if (p.patch.empty() || p.patch == "*")
		return semver(p.major + "." + std::to_string(std::stoul(p.minor) + 1u) + ".0-0");

	return semver(p.token);
}

class node final
{
private:
	enum class node_type { op_and, op_or, op_eq, op_lt, op_le, op_gt, op_ge };

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

	bool eval(const semver & v) const noexcept
	{
		switch (type_) {
			case node_type::op_and:
				return left_->eval(v) && right_->eval(v);
			case node_type::op_or:
				return left_->eval(v) || right_->eval(v);
			case node_type::op_eq:
				return v == *version_;
			case node_type::op_lt:
				return v < *version_;
			case node_type::op_le:
				return v <= *version_;
			case node_type::op_gt:
				return v > *version_;
			case node_type::op_ge:
				return v >= *version_;
		}
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

class range final
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
		: lex_(trim(s))
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

	bool good_ = false;

	detail::lexer lex_;
	detail::lexer::token token_ = detail::lexer::token::eof;
	detail::lexer::token next_ = detail::lexer::token::eof;
	detail::lexer::parts token_text_ = {};
	detail::lexer::parts next_text_ = {};

	std::unique_ptr<detail::node> ast_;
	std::deque<std::unique_ptr<detail::node>> ast_stack_;

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

	lexer::string_type token_text() noexcept { return token_text_.token; }

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
			// TODO: handle AST
			advance(); // logial-or
			std::cout << "### logical-or\n";
			parse_range();
		}

		good_ = !is_error(token_);
	}

	void parse_range() noexcept
	{
		if (is_partial(token_) && is_dash(next_)) {
			auto s = token_text();
			advance(); // partial
			advance(); // dash
			if (is_partial(token_)) {
				// TODO: handle bounds
				// TODO: handle AST
				std::cout << "### hyphen ["<< s <<"] - ["<<token_text()<<"]\n";
				advance();
				return;
			}
			error();
			return;
		}

		// TODO: handle AST with implicit AND

		while (!is_eof(token_)) {
			if (is_caret(token_)) {
				// TODO: handle bounds
				// TODO: handle AST
				std::cout << "### caret partial ["<<token_text()<<"]\n";
				advance();
				continue;
			}
			if (is_tilde(token_)) {
				// TODO: handle bounds
				// TODO: handle AST
				std::cout << "### tilde partial ["<<token_text()<<"]\n";
				advance();
				continue;
			}
			if (is_op(token_)) {
				// TODO: handle bounds
				// TODO: handle AST
				std::cout << "### op partial ["<<token_text()<<"]\n";
				advance();
				continue;
			}
			if (is_partial(token_)) {
				// TODO: handle bounds
				// TODO: handle AST
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

