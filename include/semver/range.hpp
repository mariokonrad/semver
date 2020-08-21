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

	struct parts {
		string_type token = {}; // full token

		string_type version = {}; // version without op/tilde/caret

		// indivisual parts
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

	void store()
	{
		store(parts_.token, start_);
		store(parts_.version, start_ + parts_.op.size());
	}

	void store(string_type & s, const char_type * start)
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

	token lex_caret_partial()
	{
		scan_op();
		scan_partial();
		store();
		return (!error_) ? token::caret_partial : token::error;
	}

	token lex_tilde_partial()
	{
		scan_op();
		scan_partial();
		store();
		return (!error_) ? token::tilde_partial : token::error;
	}

	token lex_op_partial()
	{
		scan_op();
		scan_partial();
		store();
		return (!error_) ? token::op_partial : token::error;
	}

	token lex_partial()
	{
		scan_partial();
		store();
		return (!error_) ? token::partial : token::error;
	}

	void scan_op()
	{
		const char_type * p = cursor_;
		if (is_eq() || is_lt() || is_gt() || is_tilde() || is_caret())
			advance(1);
		else
			advance(2);
		store(parts_.op, p);
	}

	void scan_major()
	{
		const char_type * p = cursor_;
		scan_partial_version();
		store(parts_.major, p);
	}

	void scan_minor()
	{
		const char_type * p = cursor_;
		scan_partial_version();
		store(parts_.minor, p);
	}

	void scan_patch()
	{
		const char_type * p = cursor_;
		scan_partial_version();
		store(parts_.patch, p);
	}

	void scan_prerelease()
	{
		const char_type * p = cursor_;
		scan_dot_separated_identifier();
		store(parts_.prerelease, p);
	}

	void scan_build()
	{
		const char_type * p = cursor_;
		scan_dot_separated_identifier();
		store(parts_.build, p);
	}

	void scan_partial()
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

inline semver lower_bound(const lexer::parts & p)
{
	// TODO: to be optimized, re-parse semver? meh...

	if (p.major.empty() || p.major == "*")
		return semver::min();

	if (p.minor.empty() || p.minor == "*")
		return semver(p.major + ".0.0");

	if (p.patch.empty() || p.patch == "*")
		return semver(p.major + "." + p.minor + ".0");

	return semver(p.token.substr(
		p.op.size(), p.token.size() - p.op.size() - p.build.size())); // cut op and build
}

inline semver upper_bound(const lexer::parts & p)
{
	// TODO: to be optimized, re-parse semver? meh...

	if (p.major.empty() || p.major == "*")
		return semver::max();

	if (p.op == "^" && p.major != "0")
		return semver(std::to_string(std::stoul(p.major) + 1u) + ".0.0-0");

	if (p.minor.empty() || p.minor == "*")
		return semver(std::to_string(std::stoul(p.major) + 1u) + ".0.0-0");

	if (p.op == "^" && p.minor != "0")
		return semver(p.major + "." + std::to_string(std::stoul(p.minor) + 1u) + ".0-0");

	if (p.patch.empty() || p.patch == "*" || p.op == "~")
		return semver(p.major + "." + std::to_string(std::stoul(p.minor) + 1u) + ".0-0");

	if (p.op == "^" && p.patch != "0")
		return semver(
			p.major + "." + p.minor + "." + std::to_string(std::stoul(p.patch) + 1u) + "-0");

	return semver(p.token.substr(
		p.op.size(), p.token.size() - p.op.size() - p.build.size())); // cut op and build
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
				// shortcut behavior
				if (!left_->eval(v))
					return false;
				if (!right_->eval(v))
					return false;
				return true;

			case node_type::op_or:
				// shortcut behavior
				if (left_->eval(v))
					return true;
				if (right_->eval(v))
					return true;
				return false;

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

	void optimize()
	{
		optimize_r(*this);
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

	bool is_leaf() const noexcept { return !left_ && !right_; }

	static void optimize_r(node & n) noexcept
	{
		// prequisite: the node is either a leaf or has both subtrees.
		//             this is guaranteed by the factory member functions.

		if (n.is_leaf())
			return;

		const bool right_is_leaf = n.right_->is_leaf();
		if (n.left_->is_leaf() && right_is_leaf)
			return;

		// swap leaf to the right
		if (right_is_leaf) {
			using std::swap;
			swap(n.left_, n.right_);
		}

		optimize_r(*n.left_);
		optimize_r(*n.right_);
	}

	friend void dump(std::ostream &, const node &, int);
};

inline void dump(std::ostream & os, const node & n, int indent) // TODO: temporary
{
	switch (n.type_) {
		case node::node_type::op_and:
			os << "&&";
			break;
		case node::node_type::op_or:
			os << "||";
			break;
		case node::node_type::op_eq:
			os << "==";
			break;
		case node::node_type::op_lt:
			os << "< ";
			break;
		case node::node_type::op_le:
			os << "<=";
			break;
		case node::node_type::op_gt:
			os << "> ";
			break;
		case node::node_type::op_ge:
			os << ">=";
			break;
	}

	if (n.version_) {
		os << *n.version_;
	} else {
		os << '\n';
		os << std::string(indent * 4, ' ') << "  l: ";
		dump(os, *n.left_, indent + 1);
		os << std::string(indent * 4, ' ') << "  r: ";
		dump(os, *n.right_, indent + 1);
	}

	os << '\n';
}
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

		if (!ast_.empty())
			ast_.back()->optimize();
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

	bool satisfies(const semver & v) const noexcept
	{
		if (ast_.empty())
			return false;

		assert(ast_.size() == 1u);
		return ast_.back()->eval(v);
	}

	semver max_satisfying(const std::vector<semver> & versions) const noexcept
	{
		// TODO: benchmark what is better: linear search or sort before
		return satisfies_if(begin(versions), end(versions), std::greater<>{});
	}

	semver min_satisfying(const std::vector<semver> & versions) const noexcept
	{
		// TODO: benchmark what is better: linear search or sort before
		return satisfies_if(begin(versions), end(versions), std::less<>{});
	}

	// TODO: really keep this function? It's basically just noise.
	bool outside(const semver & v) const noexcept { return !satisfies(v); }

private:
	using lexer = detail::lexer;
	using token = lexer::token;

	bool good_ = false;

	detail::lexer lex_;
	detail::lexer::token token_ = detail::lexer::token::eof;
	detail::lexer::token next_ = detail::lexer::token::eof;
	detail::lexer::parts token_text_ = {};
	detail::lexer::parts next_text_ = {};

	using node = detail::node;
	std::deque<std::unique_ptr<node>> ast_;

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
			parse_range();

			assert(ast_.size() > 1);
			auto r = std::move(ast_.back()); ast_.pop_back();
			auto l = std::move(ast_.back()); ast_.pop_back();
			ast_.push_back(std::make_unique<node>(node::create_or(std::move(l), std::move(r))));
		}

		// there might be an implicit `and` left here
		if (ast_.size() > 1) {
			assert(ast_.size() == 2u);
			auto r = std::move(ast_.back()); ast_.pop_back();
			auto l = std::move(ast_.back()); ast_.pop_back();
			ast_.push_back(std::make_unique<node>(node::create_and(std::move(l), std::move(r))));
		}

		good_ = !is_error(token_);
	}

	void parse_range() noexcept
	{
		if (is_partial(token_) && is_dash(next_)) {
			const auto first = token_text_;
			advance(); // partial
			advance(); // dash
			if (is_partial(token_)) {
				ast_.push_back(std::make_unique<node>(node::create_and(
					std::make_unique<node>(node::create_ge(lower_bound(first))),
					std::make_unique<node>(node::create_le(lower_bound(token_text_)))
					)));
				advance();
				return;
			}
			error();
			return;
		}

		while (!is_eof(token_) && !is_logical_or(token_)) {

			// implicit `and`
			if (ast_.size() > 1) {
				auto r = std::move(ast_.back()); ast_.pop_back();
				auto l = std::move(ast_.back()); ast_.pop_back();
				ast_.push_back(std::make_unique<node>(node::create_and(std::move(l), std::move(r))));
			}

			if (is_caret(token_) || is_tilde(token_)) {
				auto l = lower_bound(token_text_);
				auto u = upper_bound(token_text_);
				if (l == u) {
					ast_.push_back(std::make_unique<node>(node::create_eq(l)));
				} else {
					ast_.push_back(std::make_unique<node>(node::create_and(
						std::make_unique<node>(node::create_ge(l)),
						std::make_unique<node>(node::create_lt(u)))));
				}
				advance();
				continue;
			}
			if (is_op(token_)) {
				// TODO: refactoring
				if (token_text_.op == "<")
					ast_.push_back(
						std::make_unique<node>(node::create_lt(lower_bound(token_text_))));
				if (token_text_.op == "<=")
					ast_.push_back(
						std::make_unique<node>(node::create_le(lower_bound(token_text_))));
				if (token_text_.op == ">")
					ast_.push_back(
						std::make_unique<node>(node::create_gt(lower_bound(token_text_))));
				if (token_text_.op == ">=")
					ast_.push_back(
						std::make_unique<node>(node::create_ge(lower_bound(token_text_))));
				if (token_text_.op == "=")
					ast_.push_back(
						std::make_unique<node>(node::create_eq(lower_bound(token_text_))));
				advance();
				continue;
			}
			if (is_partial(token_)) {
				ast_.push_back(
					std::make_unique<node>(node::create_eq(lower_bound(token_text_))));
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

