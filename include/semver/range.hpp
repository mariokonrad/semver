#ifndef SEMVER_RANGE_HPP
#define SEMVER_RANGE_HPP

#include "detail/range_lexer.hpp"
#include "detail/range_node.hpp"
#include <semver/semver.hpp>
#include <sstream>
#include <memory>
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
		if (ast_.empty())
			return false;

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

	bool outside(const semver & v) const noexcept { return !satisfies(v); }

private:
	using lexer = detail::lexer;
	using token = detail::lexer::token;

	bool good_ = false;

	// TODO: separate the use of lexer to a range factory or range parser
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
		std::sort(begin(v), end(v),
			[](const auto & a, const auto & b) { return node::less(*a, *b); });
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

	void ast_push(node && n) { ast_push(std::make_unique<node>(std::move(n))); }
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
			ast_push(node::create_ge(lower_bound(token_text_)));
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
			ast_push(node::create_or(std::move(a), std::move(b)));
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

				// according to the examples provided here: https://github.com/npm/node-semver#versions,
				// use lower_bound/le if it was a complete partial, upper_bound/lt otherwise
				auto u = token_text_.full_version ? lower_bound(token_text_)
												  : upper_bound(token_text_);

				if (l == u) {
					ast_push(node::create_eq(l));
				} else {
					if (l > u)
						std::swap(l, u);
					if (token_text_.full_version) {
						ast_push(node::create_and(node::create_ge(l), node::create_le(u)));
					} else {
						ast_push(node::create_and(node::create_ge(l), node::create_lt(u)));
					}
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
					ast_push(node::create_eq(l));
				} else if (u == semver::max()) {
					ast_push(node::create_ge(l));
				} else {
					ast_push(node::create_and(node::create_ge(l), node::create_lt(u)));
				}
				advance();
				continue;
			}
			if (is_op(token_)) {
				// TODO: refactoring
				if (token_text_.op == "<")
					ast_push(node::create_lt(lower_bound(token_text_)));
				if (token_text_.op == "<=")
					ast_push(node::create_le(lower_bound(token_text_)));
				if (token_text_.op == ">")
					ast_push(node::create_gt(lower_bound(token_text_)));
				if (token_text_.op == ">=")
					ast_push(node::create_ge(lower_bound(token_text_)));
				if (token_text_.op == "=")
					ast_push(node::create_eq(lower_bound(token_text_)));
				advance();
				continue;
			}
			if (is_partial(token_)) {
				// if the version contains wildcards, handle it as range
				if (token_text_.full_version) {
					ast_push(node::create_eq(lower_bound(token_text_)));
				} else {
					auto l = lower_bound(token_text_);
					auto u = upper_bound(token_text_);
					if (l == u) {
						ast_push(node::create_eq(l));
					} else if (u == semver::max()) {
						ast_push(node::create_ge(l));
					} else {
						ast_push(node::create_and(node::create_ge(l), node::create_lt(u)));
					}
				}
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
			ast_push(node::create_and(std::move(v)));
		}
	}

	friend bool operator==(const range & r1, const range & r2) noexcept
	{
		if (r1.ast_.size() != r2.ast_.size())
			return false;

		return std::equal(begin(r1.ast_), end(r1.ast_), begin(r2.ast_),
			[](const auto & a, const auto & b) { return *a == *b; });
	}

	friend bool operator!=(const range & r1, const range & r2) noexcept { return !(r1 == r2); }

	friend std::string to_string(const range &);
};

inline std::string to_string(const range & r)
{
	std::ostringstream os;

	bool first = true;
	for (const auto & n : r.ast_) {
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

