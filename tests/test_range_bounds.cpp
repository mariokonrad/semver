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
		lexer::parts{"1.2._", "", "1", "2", "*", "", ""},
		lexer::parts{"1.*.3", "", "1", "*", "3", "", ""},
		lexer::parts{"1.*.*", "", "1", "*", "*", "", ""},
		lexer::parts{"*.2.3", "", "*", "2", "3", "", ""},
		lexer::parts{"*.2._", "", "*", "2", "*", "", ""},
		lexer::parts{"*.*.3", "", "*", "*", "3", "", ""},
		lexer::parts{"*.*.*", "", "*", "*", "*", "", ""},
	};
	// clang-format on

	for (const auto & p : tab) {
		EXPECT_TRUE(::semver::detail::lower_bound(p).ok()) << "token=" << p.token;
		EXPECT_TRUE(::semver::detail::upper_bound(p).ok()) << "token=" << p.token;
	}
}

TEST_F(test_range_bounds, xrange_major_only)
{
	const lexer::parts p {"1", "", "1", "", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p)) << "token=" << p.token;
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
	const lexer::parts p {"1.*", "", "1", "*", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_only)
{
	const lexer::parts p {"*.2.3", "", "*", "2", "3", "", ""};

	EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_only)
{
	const lexer::parts p {"1.*.3", "", "1", "*", "3", "", ""};

	EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_patch_only)
{
	const lexer::parts p {"1.2.*", "", "1", "2", "*", "", ""};

	EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(p));
	EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_and_patch)
{
	const lexer::parts p {"1.*.*", "", "1", "*", "*", "", ""};

	EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_and_minor_and_patch)
{
	const lexer::parts p {"*.*.*", "", "*", "*", "*", "", ""};

	EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(p)) << "token=" << p.token;
}

// TODO: tilde ranges

// TODO: caret ranges
}

