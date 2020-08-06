#ifndef SEMVER_HPP
#define SEMVER_HPP

#include <algorithm>
#include <charconv>
#include <string>
#include <string_view>
#include <stdexcept>
#include <cassert>

namespace semver {
inline namespace v1 {

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

private:
	struct parse_error : std::runtime_error { using runtime_error::runtime_error; };

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
		return std::string_view{start, static_cast<std::string_view::size_type>(end - start)};
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

	void advance(int n) noexcept
	{
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
			throw parse_error{"identifier"};
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

inline int compare(const semver &, const semver &) noexcept
{
	// TODO: implementation
	return 0;
}

inline std::string to_string(const semver &)
{
	// TODO: implementation
	return {};
}

inline bool operator==(const semver &, const semver &) noexcept
{
	// TODO: implementation
	return false;
}

inline bool operator!=(const semver &, const semver &) noexcept
{
	// TODO: implementation
	return false;
}

inline bool operator<(const semver &, const semver &) noexcept
{
	// TODO: implementation
	return false;
}

inline bool operator<=(const semver &, const semver &) noexcept
{
	// TODO: implementation
	return false;
}

inline bool operator>(const semver &, const semver &) noexcept
{
	// TODO: implementation
	return false;
}

inline bool operator>=(const semver &, const semver &) noexcept
{
	// TODO: implementation
	return false;
}
}
}

#endif
