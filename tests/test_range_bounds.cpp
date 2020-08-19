#include <semver/range.hpp>
#include <gtest/gtest.h>
#include <array>

namespace
{
using ::semver::semver;
using ::semver::detail::lexer;

class test_range_bounds : public ::testing::Test
{
};

TEST_F(test_range_bounds, xrange_empty)
{
	const lexer::parts p;

	EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_position)
{
	// clang-format off
	static const std::array<lexer::parts, 8> tab = {
		lexer::parts{"1.2.3", "", "1", "2", "3", "", ""},
		lexer::parts{"1.2._", "", "1", "2", "_", "", ""},
		lexer::parts{"1._.3", "", "1", "_", "3", "", ""},
		lexer::parts{"1._._", "", "1", "_", "_", "", ""},
		lexer::parts{"_.2.3", "", "_", "2", "3", "", ""},
		lexer::parts{"_.2._", "", "_", "2", "_", "", ""},
		lexer::parts{"_._.3", "", "_", "_", "3", "", ""},
		lexer::parts{"_._._", "", "_", "_", "_", "", ""},
	};
	// clang-format on

	for (const char * p = "*xX\0"; p && *p; ++p) {
		auto modify = [w=*p](std::string_view::value_type c){ return (c == '_') ? w : c; };

		std::array<lexer::parts, 8> t = tab;
		for (auto & e : t) {
			std::transform(begin(e.token), end(e.token), begin(e.token), modify);
			std::transform(begin(e.major), end(e.major), begin(e.major), modify);
			std::transform(begin(e.minor), end(e.minor), begin(e.minor), modify);
			std::transform(begin(e.patch), end(e.patch), begin(e.patch), modify);
		}

		EXPECT_TRUE(::semver::detail::lower_bound(t[0]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[1]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[2]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[3]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[4]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[5]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[6]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[7]).ok());
	}
}

TEST_F(test_range_bounds, xrange_major_only)
{
	const lexer::parts p {"1", "", "1", "", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_major_minor_only)
{
	const lexer::parts p {"1.2", "", "1", "2", "", "", ""};

	EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_major)
{
	const lexer::parts p {"*", "", "*", "", "", "", ""};

	EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor)
{
	{
		const lexer::parts p {"1.*", "", "1", "*", "", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.x", "", "1", "x", "", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.X", "", "1", "X", "", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_only)
{
	{
		const lexer::parts p {"*.2.3", "", "*", "2", "3", "", ""};

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"x.2.3", "", "x", "2", "3", "", ""};

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"X.2.3", "", "X", "2", "3", "", ""};

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_only)
{
	{
		const lexer::parts p {"1.*.3", "", "1", "*", "3", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.x.3", "", "1", "x", "3", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.X.3", "", "1", "X", "3", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_patch_only)
{
	{
		const lexer::parts p {"1.2.*", "", "1", "2", "*", "", ""};

		EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.2.x", "", "1", "2", "x", "", ""};

		EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.2.X", "", "1", "2", "X", "", ""};

		EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_and_patch)
{
	{
		const lexer::parts p {"1.*.*", "", "1", "*", "*", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.x.x", "", "1", "x", "x", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"1.X.X", "", "1", "X", "X", "", ""};

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_and_minor_and_patch)
{
	{
		const lexer::parts p {"*.*.*", "", "*", "*", "*", "", ""};

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"x.x.x", "", "x", "x", "x", "", ""};

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
	}
	{
		const lexer::parts p {"X.X.X", "", "X", "X", "X", "", ""};

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
	}
}

// TODO: tilde ranges

// TODO: caret ranges
}

