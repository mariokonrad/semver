#ifndef SEMVER_RANGE_HPP
#define SEMVER_RANGE_HPP

#include "detail/range_lexer.hpp"
#include <semver/semver.hpp>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
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
public:
	enum class type { op_and, op_or, op_eq, op_lt, op_le, op_gt, op_ge };

	~node() = default;

	node(const node &) = delete;
	node & operator=(const node &) = delete;

	node(node &&) = default;
	node & operator=(node &&) = default;

	static node create_and(std::unique_ptr<node> first, std::unique_ptr<node> second)
	{
		std::vector<std::unique_ptr<node>> v;
		v.reserve(2);
		v.push_back(std::move(first));
		v.push_back(std::move(second));
		return node(type::op_and, std::move(v));
	}

	static node create_and(std::vector<std::unique_ptr<node>> && v)
	{
		return node(type::op_and, std::move(v));
	}

	static node create_or(std::unique_ptr<node> first, std::unique_ptr<node> second)
	{
		std::vector<std::unique_ptr<node>> v;
		v.reserve(2);
		v.push_back(std::move(first));
		v.push_back(std::move(second));
		return node(type::op_or, std::move(v));
	}

	static node create_eq(const semver & s) { return {type::op_eq, s}; }
	static node create_lt(const semver & s) { return {type::op_lt, s}; }
	static node create_le(const semver & s) { return {type::op_le, s}; }
	static node create_gt(const semver & s) { return {type::op_gt, s}; }
	static node create_ge(const semver & s) { return {type::op_ge, s}; }

	bool eval(const semver & v) const noexcept
	{
		switch (type_) {
			case type::op_and:
				// shortcut behavior
				for (const auto & n : nodes_)
					if (!n->eval(v))
						return false;
				return true;

			case type::op_or:
				// shortcut behavior
				for (const auto & n : nodes_)
					if (n->eval(v))
						return true;
				return false;

			case type::op_eq:
				return v == *version_;
			case type::op_lt:
				return v < *version_;
			case type::op_le:
				return v <= *version_;
			case type::op_gt:
				return v > *version_;
			case type::op_ge:
				return v >= *version_;
		}
		return false;
	}

	bool is_leaf() const noexcept { return nodes_.empty(); }
	type get_type() const { return type_; }
	std::optional<semver> get_version() const { return version_; }

	template <typename Visitor> void visit_postfix(Visitor v) const
	{
		for (const auto & n : nodes_)
			n->visit_postfix(v);
		v(*this);
	}

	template <typename Visitor> void visit_prefix(Visitor v) const
	{
		v(*this);
		for (const auto & n : nodes_)
			n->visit_prefix(v);
	}

private:
	type type_;
	std::optional<semver> version_;
	std::vector<std::unique_ptr<node>> nodes_;

	node(type t, const semver & s)
		: type_(t)
		, version_(s)
	{
	}

	node(type t, std::vector<std::unique_ptr<node>> && v)
		: type_(t)
		, nodes_(std::move(v))
	{
		std::sort(
			begin(nodes_), end(nodes_), [](const auto & a, const auto & b) { return *a < *b; });
	}

	friend std::ostream & dump(std::ostream &, const node &, int);
	friend void collect_leafs_and_andnodes(std::vector<std::unique_ptr<node>> &, node &);

	friend bool operator==(const node & a, const node & b)
	{
		return (a.type_ == b.type_) && (a.version_ == b.version_) && (a.nodes_ == b.nodes_);
	}

	friend bool operator!=(const node & a, const node & b) { return !(a == b); }

	friend bool operator<(const node & a, const node & b)
	{
		if (a.is_leaf() && b.is_leaf())
			return *a.version_ < *b.version_;
		if (a.is_leaf() && !b.is_leaf())
			return true;
		if (!a.is_leaf() && b.is_leaf())
			return false;
		if (!a.is_leaf() && !b.is_leaf())
			return a.nodes_ < b.nodes_;
		return false;
	}
};

inline std::ostream & dump_op(std::ostream & os, const node & n) // TODO: temporary
{
	switch (n.get_type()) {
		case node::type::op_and:
			os << "&&";
			break;
		case node::type::op_or:
			os << "||";
			break;
		case node::type::op_eq:
			os << "==";
			break;
		case node::type::op_lt:
			os << "< ";
			break;
		case node::type::op_le:
			os << "<=";
			break;
		case node::type::op_gt:
			os << "> ";
			break;
		case node::type::op_ge:
			os << ">=";
			break;
	}
	return os;
}

inline void collect_leafs_and_andnodes(std::vector<std::unique_ptr<node>> & v, node & n)
{
	assert(n.get_type() == node::type::op_or);

	for (auto && p : n.nodes_) {
		if (p->get_type() == node::type::op_or) {
			collect_leafs_and_andnodes(v, *p);
		} else if (p->get_type() == node::type::op_and) {
			v.push_back(std::move(p));
		} else {
			v.push_back(std::move(p));
		}
	}
}

inline std::ostream & dump(std::ostream & os, const node & n, int indent) // TODO: temporary
{
	dump_op(os, n);
	if (n.version_) {
		os << *n.version_;
	} else {
		os << '\n';
		for (const auto & child : n.nodes_) {
			os << std::string(indent * 4, ' ') << "  n: ";
			dump(os, *child, indent + 1);
		}
	}

	return os << '\n';
}
}

static inline std::string trim(std::string s)
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

		assert(ast_.size() <= 1);
		if (!ast_.empty())
			flatten_ast();
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
			return true;

		// all nodes in AST are implicit `or`
		for (const auto & n : ast_)
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
	std::vector<std::unique_ptr<node>> ast_;

	void flatten_ast()
	{
		if (ast_back()->is_leaf())
			return;

		if (ast_back()->get_type() == node::type::op_and)
			return;

		assert(ast_back()->get_type() == node::type::op_or);

		// transform AST into a list of `or` connected nodes, simple ones frist.
		// this makes the structure of the AST more approachable for other algorithms
		// like comparing ranges.

		std::vector<std::unique_ptr<node>> v;
		collect_leafs_and_andnodes(v, *ast_back());
		std::sort(begin(v), end(v), [](const auto & a, const auto & b) { return *a < *b; });
		ast_ = std::move(v);
	}

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

	std::unique_ptr<node> & ast_back() { return ast_.back(); }
	const std::unique_ptr<node> & ast_back() const { return ast_.back(); }

	void ast_push(std::unique_ptr<node> n) { ast_.push_back(std::move(n)); }

	std::unique_ptr<node> ast_pop()
	{
		auto n = std::move(ast_back());
		ast_.erase(prev(end(ast_)));
		return n;
	}


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
			auto b = ast_pop();
			auto a = ast_pop();
			ast_push(std::make_unique<node>(node::create_or(std::move(a), std::move(b))));
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
				auto l = lower_bound(first);
				auto u = lower_bound(token_text_);
				if (l == u) {
					ast_push(std::make_unique<node>(node::create_eq(l)));
				} else {
					if (l > u)
						std::swap(l, u);
					ast_push(std::make_unique<node>(
						node::create_and(std::make_unique<node>(node::create_ge(l)),
							std::make_unique<node>(node::create_le(u)))));
				}
				advance();
				return;
			}
			error();
			return;
		}

		int partial_count = 0; // how may partials are participating in implicit and
		while (!is_eof(token_) && !is_logical_or(token_)) {
			++partial_count;

			if (is_caret(token_) || is_tilde(token_)) {
				auto l = lower_bound(token_text_);
				auto u = upper_bound(token_text_);
				if (l == u) {
					ast_push(std::make_unique<node>(node::create_eq(l)));
				} else {
					ast_push(std::make_unique<node>(
						node::create_and(std::make_unique<node>(node::create_ge(l)),
							std::make_unique<node>(node::create_lt(u)))));
				}
				advance();
				continue;
			}
			if (is_op(token_)) {
				// TODO: refactoring
				if (token_text_.op == "<")
					ast_push(std::make_unique<node>(node::create_lt(lower_bound(token_text_))));
				if (token_text_.op == "<=")
					ast_push(std::make_unique<node>(node::create_le(lower_bound(token_text_))));
				if (token_text_.op == ">")
					ast_push(std::make_unique<node>(node::create_gt(lower_bound(token_text_))));
				if (token_text_.op == ">=")
					ast_push(std::make_unique<node>(node::create_ge(lower_bound(token_text_))));
				if (token_text_.op == "=")
					ast_push(std::make_unique<node>(node::create_eq(lower_bound(token_text_))));
				advance();
				continue;
			}
			if (is_partial(token_)) {
				ast_push(std::make_unique<node>(node::create_eq(lower_bound(token_text_))));
				advance();
				continue;
			}

			error();
			return;
		}

		// are there multiple leafs and therefore an implicit 'and'?
		if (partial_count > 1) {
			std::vector<std::unique_ptr<node>> v;
			for (; partial_count > 0 && ast_back()->is_leaf(); --partial_count)
				v.push_back(std::move(ast_pop()));
			ast_push(std::make_unique<node>(node::create_and(std::move(v))));
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

