#ifndef SEMVER_HPP
#define SEMVER_HPP

#include <algorithm>
#include <charconv>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cassert>

namespace semver
{
inline namespace v1
{

// Implementation of semver 2.0.0
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
		last_ = data_.data() + data_.size();
		cursor_ = data_.data();

		try {
			parse_valid_semver();
			good_ = cursor_ == last_;
		} catch (parse_error &) {
			// left blank
		}
	}

	unsigned long major() const noexcept { return major_; }
	unsigned long minor() const noexcept { return minor_; }
	unsigned long patch() const noexcept { return patch_; }
	const std::string_view build() const noexcept { return build_; }
	const std::string_view prerelease() const noexcept { return prerelease_; }

	bool ok() const noexcept { return good_; }
	explicit operator bool() const noexcept { return ok(); }

	std::string str() const
	{
		auto s = std::to_string(major()) + '.' + std::to_string(minor()) + '.'
			+ std::to_string(patch());
		const auto pr = prerelease();
		const auto b = build();
		if (!pr.empty() || !b.empty()) {
			s.reserve(s.size() + pr.size() + (!!pr.size()) + b.size() + (!!b.size()));
			if (!pr.empty()) {
				s += '-';
				s.append(pr.data(), pr.size());
			}
			if (!b.empty()) {
				s += '+';
				s.append(b.data(), b.size());
			}
		}
		return s;
	}

private:
	struct parse_error : std::runtime_error {
		using runtime_error::runtime_error;
	};

	std::string data_;
	const char_type * last_ = {};
	const char_type * start_ = {};
	const char_type * cursor_ = {};
	bool good_ = false;

	unsigned long major_ = {};
	unsigned long minor_ = {};
	unsigned long patch_ = {};
	std::string_view prerelease_ = {};
	std::string_view build_ = {};

	static std::string_view token(const char_type * start, const char_type * end) noexcept
	{
		return std::string_view {start, static_cast<std::string_view::size_type>(end - start)};
	}

	void parse_valid_semver()
	{
		parse_version_core();
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

	void advance(int n) noexcept { cursor_ += std::min(n, static_cast<int>(last_ - cursor_)); }

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
		std::from_chars(start_, cursor_, major_);
	}

	void parse_minor()
	{
		start_ = cursor_;
		parse_numeric_identifier();
		std::from_chars(start_, cursor_, minor_);
	}

	void parse_patch()
	{
		start_ = cursor_;
		parse_numeric_identifier();
		std::from_chars(start_, cursor_, patch_);
	}

	void parse_dot()
	{
		if (is_dot(cursor_)) {
			advance(1);
			return;
		}
		throw parse_error {"dot"};
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
		parse_identifier();
		while (is_dot(cursor_)) {
			parse_dot();
			parse_identifier();
		}
	}

	void parse_dot_separated_prerelease_identifier()
	{
		parse_identifier();
		while (is_dot(cursor_)) {
			parse_dot();
			parse_identifier();
		}
	}

	void parse_identifier()
	{
		if (!(is_letter(cursor_) || is_digit(cursor_) || is_dash(cursor_)))
			throw parse_error {"identifier"};
		while (is_letter(cursor_) || is_digit(cursor_) || is_dash(cursor_))
			advance(1);
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
		throw parse_error {"numeric_identifier"};
	}

	void parse_positive_digit()
	{
		if (is_positive_digit(cursor_)) {
			advance(1);
			return;
		}
		throw parse_error {"positive_digit"};
	}

	void parse_digits()
	{
		if (!is_digit(cursor_))
			throw parse_error {"digits"};
		while (is_digit(cursor_))
			advance(1);
	}

	// low level primitives, must check for eof

	bool is_eof(const char_type * p) const noexcept { return (p >= last_) || (*p == '\x00'); }

	bool is_dot(const char_type * p) const noexcept { return !is_eof(p) && (*p == '.'); }

	bool is_plus(const char_type * p) const noexcept { return !is_eof(p) && (*p == '+'); }

	bool is_positive_digit(const char_type * p) const noexcept
	{
		return !is_eof(p) && ((*p >= '1') && (*p <= '9'));
	}

	bool is_digit(const char_type * p) const noexcept
	{
		return !is_eof(p) && ((*p >= '0') && (*p <= '9'));
	}

	bool is_zero(const char_type * p) const noexcept { return !is_eof(p) && (*p == '0'); }

	bool is_dash(const char_type * p) const noexcept { return !is_eof(p) && (*p == '-'); }

	bool is_letter(const char_type * p) const noexcept
	{
		return !is_eof(p) && (((*p >= 'A') && (*p <= 'Z')) || ((*p >= 'a') && (*p <= 'z')));
	}
};

inline std::string to_string(const semver & v)
{
	return v.str();
}

inline bool operator==(const semver & v1, const semver & v2) noexcept
{
	return true && (v1.major() == v2.major()) && (v1.minor() == v2.minor())
		&& (v1.patch() == v2.patch()) && (v1.prerelease() == v2.prerelease());
}

inline bool operator!=(const semver & v1, const semver & v2) noexcept
{
	return !(v1 == v2);
}

inline bool operator<(const semver & v1, const semver & v2) noexcept
{
	// major, minor and patch are first priority to compare in descending order. if one
	// of them is different, it is already decided.
	if (v1.major() < v2.major())
		return true;
	if (v1.minor() < v2.minor())
		return true;
	if (v1.patch() < v2.patch())
		return true;

	if (v1.major() > v2.major())
		return false;
	if (v1.minor() > v2.minor())
		return false;
	if (v1.patch() > v2.patch())
		return false;

	const auto p1 = v1.prerelease();
	const auto p2 = v2.prerelease();

	// if one has a prerelease the other not, the one with is smaller
	if (!p1.empty() && p2.empty())
		return true;
	if (p1.empty() && !p2.empty())
		return false;

	// compare fields of prerelease individually, separated by '.'
	// rules from semver.org, literally or shortened:
	//
	//   - pure numerically are compared numerically
	//   - alphanumeric are compared lexically
	//   - pure numerically has always lower precedence than alphanumerically
	//   - larger set of fields has a higher precedence than the smaller set, if all of
	//     preceeding identifiers are equal, example:
	//     1.0.0-alpha < 1.0.0-alpha.1 < 1.0.0-alpha.beta < 1.0.0-beta < 1.0.0-beta.2
	//     < 1.0.0-beta.11 < 1.0.0-rc.1 < 1.0.0
	//
	std::string_view::size_type c1 = 0u;
	std::string_view::size_type c2 = 0u;
	for (;;) {

		const auto c1e = p1.find('.', c1);
		const auto c2e = p2.find('.', c2);

		// numerical vs alphanumerical fields? alphanumerical alwasys wins
		const bool c1num = p1.substr(c1, c1e).find_first_not_of(std::string_view("0123456789"))
			== std::string_view::npos;
		const bool c2num = p2.substr(c2, c2e).find_first_not_of(std::string_view("0123456789"))
			== std::string_view::npos;
		if (c1num && !c2num)
			return true;
		if (!c1num && c2num)
			return false;

		if (c1num && c2num) {
			// pure numerical comparison

			unsigned long long p1n = 0u;
			unsigned long long p2n = 0u;

			// we ignore the return value because it was already tested to be numerical only
			std::from_chars(p1.data() + c1, p1.data() + std::min(c1e, p1.size()), p1n);
			std::from_chars(p2.data() + c2, p2.data() + std::min(c2e, p2.size()), p2n);

			if (p1n < p2n)
				return true;
			if (p1n > p2n)
				return false;
		} else {
			// alphanumerical comparison

			const auto rc = p1.compare(c1, c1e - c1, p2, c2, c2e - c2);
			if (rc < 0)
				return true;
			if (rc > 0)
				return false;
		}

		// compare number of fields, unil now all fields were the same, the one with
		// more fields is the higher version
		if ((c1e != std::string_view::npos) && (c2e == std::string_view::npos))
			return false; // p1 has more fields
		if ((c1e == std::string_view::npos) && (c2e != std::string_view::npos))
			return true; // p2 has more fields
		if ((c1e == std::string_view::npos) && (c2e == std::string_view::npos))
			return false; // both exhausted

		// all were the same until now, on to the next field
		c1 = c1e + 1u;
		c2 = c2e + 1u;
	}
}

inline bool operator<=(const semver & v1, const semver & v2) noexcept
{
	return (v1 == v2) || (v1 < v2);
}

inline bool operator>(const semver & v1, const semver & v2) noexcept
{
	return !(v1 <= v2);
}

inline bool operator>=(const semver & v1, const semver & v2) noexcept
{
	return !(v1 < v2);
}

inline int compare(const semver & v1, const semver & v2) noexcept
{
	return (v1 == v2) ? 0 : (v1 < v2) ? -1 : +1;
}
}
}

#endif
