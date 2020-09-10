#ifndef SERMVER_SEMVER_HPP
#define SERMVER_SEMVER_HPP

#include <algorithm>
#include <charconv>
#include <functional>
#include <limits>
#include <ostream>
#include <string>

namespace semver
{
inline namespace v1
{
// Implementation of semver 2.0.0
//
class semver final
{
public:
	using number_type = unsigned long;

	~semver() = default;

	semver(const semver &) = default;
	semver & operator=(const semver &) = default;

	semver(semver &&) = default;
	semver & operator=(semver &&) = default;

	semver() = default;

	semver(const std::string & s, bool loose = false)
		: data_(s)
	{
		if (loose) {
			data_.erase(std::remove_if(begin(data_), end(data_), ::isspace), end(data_));
			data_.erase(begin(data_), std::find_if(begin(data_), end(data_), ::isdigit));
		}

		last_ = data_.data() + data_.size();
		cursor_ = data_.data();

		if (!data_.empty()) {
			parse_valid_semver();
			good_ = (cursor_ == last_) && !error_;
		}
	}

	semver(number_type major, number_type minor, number_type patch)
		: major_(major)
		, minor_(minor)
		, patch_(patch)
		, good_(true)
	{
	}

	semver(
		number_type major, number_type minor, number_type patch, const std::string & prerelease)
		: major_(major)
		, minor_(minor)
		, patch_(patch)
		, prerelease_(prerelease)
		, good_(prerelease_.empty())
	{
		last_ = prerelease_.data() + prerelease_.size();
		cursor_ = prerelease_.data();

		if (!good_) {
			parse_pre_release();
			good_ = (cursor_ == last_) && !error_;
		}
	}

	number_type major() const noexcept { return major_; }
	number_type minor() const noexcept { return minor_; }
	number_type patch() const noexcept { return patch_; }
	std::string build() const noexcept { return build_; }
	std::string prerelease() const noexcept { return prerelease_; }

	bool ok() const noexcept { return good_; }
	explicit operator bool() const noexcept { return ok(); }

	std::string str() const
	{
		if (!ok())
			return "<invalid>";

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

	static semver invalid() noexcept { return semver(); }

	static semver min() noexcept { return semver(0u, 0u, 0u); }

	static semver max() noexcept
	{
		return semver(std::numeric_limits<number_type>::max(),
			std::numeric_limits<number_type>::max(), std::numeric_limits<number_type>::max());
	}

private:
	using char_type = std::string::value_type;

	const char_type * last_ = {};
	const char_type * start_ = {};
	const char_type * cursor_ = {};
	const char_type * error_ = nullptr;

	number_type major_ = {};
	number_type minor_ = {};
	number_type patch_ = {};
	std::string prerelease_ = {};
	std::string build_ = {};

	std::string data_;
	bool good_ = false;

	static std::string token(const char_type * start, const char_type * end) noexcept
	{
		return std::string(start, end);
	}

	void parse_valid_semver() noexcept
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

	void error() noexcept { error_ = cursor_; }

	void parse_version_core() noexcept
	{
		parse_major();
		parse_dot();
		parse_minor();
		parse_dot();
		parse_patch();
	}

	void parse_major() noexcept
	{
		start_ = cursor_;
		parse_numeric_identifier();
		std::from_chars(start_, cursor_, major_);
	}

	void parse_minor() noexcept
	{
		start_ = cursor_;
		parse_numeric_identifier();
		std::from_chars(start_, cursor_, minor_);
	}

	void parse_patch() noexcept
	{
		start_ = cursor_;
		parse_numeric_identifier();
		std::from_chars(start_, cursor_, patch_);
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

	void parse_numeric_identifier() noexcept
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
	bool is_dot(const char_type * p) const noexcept { return peek('.', p); }
	bool is_plus(const char_type * p) const noexcept { return peek('+', p); }
	bool is_dash(const char_type * p) const noexcept { return peek('-', p); }
	bool is_zero(const char_type * p) const noexcept { return peek('0', p); }

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

inline std::string to_string(const semver & v)
{
	return v.str();
}

inline std::ostream & operator<<(std::ostream & os, const semver & v)
{
	return os << to_string(v);
}

inline bool operator==(const semver & v1, const semver & v2) noexcept
{
	return (v1.major() == v2.major()) && (v1.minor() == v2.minor())
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
	if (v1.major() > v2.major())
		return false;

	if (v1.minor() < v2.minor())
		return true;
	if (v1.minor() > v2.minor())
		return false;

	if (v1.patch() < v2.patch())
		return true;
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
	std::string::size_type c1 = 0u;
	std::string::size_type c2 = 0u;
	for (;;) {

		const auto c1e = p1.find('.', c1);
		const auto c2e = p2.find('.', c2);

		// numerical vs alphanumerical fields? alphanumerical alwasys wins
		auto find_alpha_in_substr = [](const std::string & s, auto start_pos, auto end_pos) {
			const auto sub = s.substr(start_pos, end_pos);
			return std::find_if(begin(sub), end(sub), std::not_fn(::isdigit)) == end(sub);
		};

		const bool c1num = find_alpha_in_substr(p1, c1, c1e);
		const bool c2num = find_alpha_in_substr(p2, c2, c2e);
		if (c1num && !c2num)
			return true;
		if (!c1num && c2num)
			return false;

		if (c1num && c2num) {
			// pure numerical comparison

			semver::number_type p1n = 0u;
			semver::number_type p2n = 0u;

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
		if ((c1e != std::string::npos) && (c2e == std::string::npos))
			return false; // p1 has more fields
		if ((c1e == std::string::npos) && (c2e != std::string::npos))
			return true; // p2 has more fields
		if ((c1e == std::string::npos) && (c2e == std::string::npos))
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
