#ifndef SEMVER_HPP
#define SEMVER_HPP

#include <algorithm>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cassert>


namespace semver {
inline namespace v1 {

// BNF from semver.org
//
// <valid semver> ::= <version core>
//                  | <version core> "-" <pre-release>
//                  | <version core> "+" <build>
//                  | <version core> "-" <pre-release> "+" <build>
//
// <version core> ::= <major> "." <minor> "." <patch>
//
// <major> ::= <numeric identifier>
//
// <minor> ::= <numeric identifier>
//
// <patch> ::= <numeric identifier>
//
// <pre-release> ::= <dot-separated pre-release identifiers>
//
// <dot-separated pre-release identifiers> ::= <pre-release identifier>
//                                           | <pre-release identifier> "." <dot-separated pre-release identifiers>
//
// <build> ::= <dot-separated build identifiers>
//
// <dot-separated build identifiers> ::= <build identifier>
//                                     | <build identifier> "." <dot-separated build identifiers>
//
// <pre-release identifier> ::= <alphanumeric identifier>
//                            | <numeric identifier>
//
// <build identifier> ::= <alphanumeric identifier>
//                      | <digits>
//
// <alphanumeric identifier> ::= <non-digit>
//                             | <non-digit> <identifier characters>
//                             | <identifier characters> <non-digit>
//                             | <identifier characters> <non-digit> <identifier characters>
//
// <numeric identifier> ::= "0"
//                        | <positive digit>
//                        | <positive digit> <digits>
//
// <identifier characters> ::= <identifier character>
//                           | <identifier character> <identifier characters>
//
// <identifier character> ::= <digit>
//                          | <non-digit>
//
// <non-digit> ::= <letter>
//               | "-"
//
// <digits> ::= <digit>
//            | <digit> <digits>
//
// <digit> ::= "0"
//           | <positive digit>
//
// <positive digit> ::= "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
//
// <letter> ::= "A" | "B" | "C" | "D" | "E" | "F" | "G" | "H" | "I" | "J"
//            | "K" | "L" | "M" | "N" | "O" | "P" | "Q" | "R" | "S" | "T"
//            | "U" | "V" | "W" | "X" | "Y" | "Z" | "a" | "b" | "c" | "d"
//            | "e" | "f" | "g" | "h" | "i" | "j" | "k" | "l" | "m" | "n"
//            | "o" | "p" | "q" | "r" | "s" | "t" | "u" | "v" | "w" | "x"
//            | "y" | "z"
//
//
class semver
{
private:
	using string_type = std::string;
	using iterator_type = typename string_type::const_iterator;
	using char_type = typename string_type::value_type;

public:
	~semver() = default;

	semver(const semver &) = default;
	semver & operator=(const semver &) = default;

	semver(semver &&) = default;
	semver & operator=(semver &&) = default;

	semver(const std::string & s)
		: data_(s)
	{
		assert(!data_.empty());
		first_ = data_.data();
		last_ = data_.data() + data_.size();
		cursor_ = data_.data();

		try {
			parse();
			good_ = true;
		} catch (parse_error &) {
			// left blank
		}
	}

	std::string_view major() const noexcept { return major_; }
	std::string_view minor() const noexcept { return minor_; }
	std::string_view patch() const noexcept { return patch_; }
	std::string_view build() const noexcept { return build_; }
	std::string_view prerelease() const noexcept { return prerelease_; }

	bool ok() const noexcept { return good_; }
	explicit operator bool() const noexcept { return ok(); }

private:
	struct parse_error : std::runtime_error { using runtime_error::runtime_error; };

	std::string data_;
	const char_type * first_ = {};
	const char_type * last_ = {};
	const char_type * start_ = {};
	const char_type * cursor_ = {};
	bool good_ = false;

	std::string_view major_ = {};
	std::string_view minor_ = {};
	std::string_view patch_ = {};
	std::string_view prerelease_ = {};
	std::string_view build_ = {};

	static std::string_view token(const char_type * start, const char_type * end) noexcept
	{
		return std::string_view{start, static_cast<std::string_view::size_type>(end - start)};
	}

	void parse()
	{
		parse_version_core();
		if (is_plus(cursor_)) {
			advance(1);
			parse_build();
		}
		if (is_dash(cursor_)) {
			advance(1);
			parse_pre_release();
		}
	}

	void advance(int n) noexcept
	{
		if (cursor_ < last_)
			cursor_ += std::min(n, static_cast<int>(last_ - cursor_));
	}

	void parse_version_core()
	{
		parse_major();
		parse_dot();
		parse_minor();
		parse_dot();
		parse_patch();
	}

	void parse_major()
	{
		start_ = cursor_;
		parse_numeric_identifier();
		major_ = token(start_, cursor_);
	}

	void parse_minor()
	{
		start_ = cursor_;
		parse_numeric_identifier();
		minor_ = token(start_, cursor_);
	}

	void parse_patch()
	{
		start_ = cursor_;
		parse_numeric_identifier();
		patch_ = token(start_, cursor_);
	}

	void parse_dot()
	{
		if (is_dot(cursor_)) {
			advance(1);
			return;
		}
		throw parse_error{"parse_dot"};
	}

	void parse_build()
	{
		start_ = cursor_;
		parse_dot_separated_build_identifier();
		build_ = token(start_, cursor_);
	}

	void parse_pre_release()
	{
		start_ = cursor_;
		parse_dot_separated_prerelease_identifier();
		prerelease_ = token(start_, cursor_);
	}

	void parse_dot_separated_build_identifier()
	{
		parse_build_identifier();
		while (is_dot(cursor_)) {
			parse_dot();
			parse_build_identifier();
		}
	}

	void parse_build_identifier()
	{
		if (is_alphanumeric_identifier(cursor_)) {
			parse_alphanumeric_identiifer();
			return;
		}
		if (is_digit(cursor_)) {
			parse_digits();
			return;
		}
		throw parse_error{"build_identifier"};
	}

	void parse_dot_separated_prerelease_identifier()
	{
		parse_prerelease_identifier();
		while (is_dot(cursor_)) {
			parse_dot();
			parse_prerelease_identifier();
		}
	}

	void parse_prerelease_identifier()
	{
		if (is_alphanumeric_identifier(cursor_)) {
			parse_alphanumeric_identiifer();
			return;
		}
		if (is_numeric_identifier(cursor_)) {
			parse_numeric_identifier();
			return;
		}
		throw parse_error{"prerelease_identifier"};
	}

	void parse_alphanumeric_identiifer()
	{
		if (is_non_digit(cursor_)) {
			parse_non_digit();
			if (is_identifier_character(cursor_))
				parse_identifier_characters();
			return;
		}
		if (is_identifier_character(cursor_)) {
			parse_identifier_characters();
			parse_non_digit();
			if (is_identifier_character(cursor_))
				parse_identifier_characters();
			return;
		}
		throw parse_error{"alphanumeric_identifier"};
	}

	void parse_identifier_characters()
	{
		if (!is_identifier_character(cursor_))
			throw parse_error{"identifier_characters"};
		while (is_identifier_character(cursor_))
			advance(1);
	}

	void parse_identifier_character()
	{
		if (is_digit(cursor_)) {
			advance(1);
			return;
		}
		if (is_non_digit(cursor_)) {
			advance(1);
			return;
		}
		throw parse_error{"identifier_character"};
	}

	void parse_non_digit()
	{
		if (is_non_digit(cursor_)) {
			advance(1);
			return;
		}
		throw parse_error{"non_digit"};
	}

	void parse_numeric_identifier()
	{
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
		throw parse_error{"numeric_identifier"};
	}

	void parse_positive_digit()
	{
		if (is_positive_digit(cursor_)) {
			advance(1);
			return;
		}
		throw parse_error{"positive_digit"};
	}

	void parse_digits()
	{
		if (!is_digit(cursor_))
			throw parse_error{"digits"};
		while (is_digit(cursor_))
			advance(1);
	}

	template <class F>
	bool peek(F f, int n = 1) const noexcept
	{
		return f(cursor_ + n);
	}

	bool is_alphanumeric_identifier(const char_type * p) const noexcept
	{
		return is_non_digit(p)
			|| (is_identifier_character(p) && peek([this](const char_type * p) { return is_non_digit(p); }));
	}

	bool is_numeric_identifier(const char_type * p) const noexcept
	{
		return is_zero(p) || is_positive_digit(p);
	}

	bool is_identifier_character(const char_type * p) const noexcept
	{
		return is_digit(p) || is_non_digit(p);
	}

	bool is_non_digit(const char_type * p) const noexcept
	{
		return is_letter(p) || is_dash(p);
	}

	// low level primitives, must check for eof

	bool is_eof(const char_type * p) const noexcept
	{
		return (p >= last_) || (*p == '\x00');
	}

	bool is_dot(const char_type * p) const noexcept
	{
		return !is_eof(p) && (*p == '.');
	}

	bool is_plus(const char_type * p) const noexcept
	{
		return !is_eof(p) && (*p == '+');
	}

	bool is_positive_digit(const char_type * p) const noexcept
	{
		return !is_eof(p) && ((*p >= '1') && (*p <= '9'));
	}

	bool is_digit(const char_type * p) const noexcept
	{
		return !is_eof(p) && ((*p >= '0') && (*p <= '9'));
	}

	bool is_zero(const char_type * p) const noexcept
	{
		return !is_eof(p) && (*p == '0');
	}

	bool is_dash(const char_type * p) const noexcept
	{
		return !is_eof(p) && (*p == '-');
	}

	bool is_letter(const char_type * p) const noexcept
	{
		return !is_eof(p) && (((*p >= 'A') && (*p <= 'Z')) || ((*p >= 'a') && (*p <= 'z')));
	}
};

}
}

#endif
