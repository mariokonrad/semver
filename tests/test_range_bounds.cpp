#include <semver/range.hpp>
#include <gtest/gtest.h>
#include <array>

namespace
{
using ::semver::v1::semver;
using ::semver::v1::detail::lexer;
using ::semver::v1::detail::lower_bound;
using ::semver::v1::detail::upper_bound;
using ::semver::v1::detail::node;

class test_range_bounds : public ::testing::Test
{
};

TEST_F(test_range_bounds, xrange_empty)
{
	const lexer::parts p;

	EXPECT_EQ(semver::min(), lower_bound(p));
	EXPECT_EQ(semver::max(), upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_position)
{
	// clang-format off
	static const std::array<lexer::parts, 8> tab = {
		lexer::parts{"1.2.3", "1.2.3", true,  "", "1", "2", "3", "", ""},
		lexer::parts{"1.2.*", "1.2.*", false, "", "1", "2", "*", "", ""},
		lexer::parts{"1.*.3", "1.*.3", false, "", "1", "*", "3", "", ""},
		lexer::parts{"1.*.*", "1.*.*", false, "", "1", "*", "*", "", ""},
		lexer::parts{"*.2.3", "*.2.3", false, "", "*", "2", "3", "", ""},
		lexer::parts{"*.2.*", "*.2.*", false, "", "*", "2", "*", "", ""},
		lexer::parts{"*.*.3", "*.*.3", false, "", "*", "*", "3", "", ""},
		lexer::parts{"*.*.*", "*.*.*", false, "", "*", "*", "*", "", ""},
	};
	// clang-format on

	for (const auto & p : tab) {
		EXPECT_TRUE(lower_bound(p).ok()) << "token=" << p.token;
		EXPECT_TRUE(upper_bound(p).ok()) << "token=" << p.token;
	}
}

TEST_F(test_range_bounds, major_minor_patch)
{
	const lexer::parts p {"1.2.3", "1.2.3", true, "", "1", "2", "3", "", ""};

	EXPECT_EQ(semver("1.2.3"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.2.3"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major_minor)
{
	const lexer::parts p {"1.2", "1.2", false, "", "1", "2", "", "", ""};

	EXPECT_EQ(semver("1.2.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major)
{
	const lexer::parts p {"1", "1", false, "", "1", "", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major_minor_patch_build)
{
	const lexer::parts p {"1.2.3+1", "1.2.3+1", true, "", "1", "2", "3", "", "1"};

	EXPECT_EQ(semver("1.2.3"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.2.3"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major_minor_build)
{
	const lexer::parts p {"1.2+1", "1.2+1", false, "", "1", "2", "", "", "1"};

	EXPECT_EQ(semver("1.2.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major_build)
{
	const lexer::parts p {"1+1", "1+1", false, "", "1", "", "", "", "1"};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major_minor_patch_prerelease)
{
	const lexer::parts p {"1.2.3-1", "1.2.3-1", true, "", "1", "2", "3", "1", ""};

	EXPECT_EQ(semver("1.2.3-1"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.2.3-1"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major_minor_prerelease)
{
	const lexer::parts p {"1.2-1", "1.2-1", false, "", "1", "2", "", "1", ""};

	EXPECT_EQ(semver("1.2.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, major_prerelease)
{
	const lexer::parts p {"1-1", "1-1", false, "", "1", "", "", "1", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, xrange_major_only)
{
	const lexer::parts p {"1", "1", false, "", "1", "", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, xrange_major_minor_only)
{
	const lexer::parts p {"1.2", "1.2", false, "", "1", "2", "", "", ""};

	EXPECT_EQ(semver("1.2.0"), lower_bound(p));
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_major)
{
	const lexer::parts p {"*", "*", false, "", "*", "", "", "", ""};

	EXPECT_EQ(semver::min(), lower_bound(p));
	EXPECT_EQ(semver::max(), upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor)
{
	const lexer::parts p {"1.*", "1.*", false, "", "1", "*", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p));
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_only)
{
	const lexer::parts p {"*.2.3", "*.2.3", false, "", "*", "2", "3", "", ""};

	EXPECT_EQ(semver::min(), lower_bound(p));
	EXPECT_EQ(semver::max(), upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_only)
{
	const lexer::parts p {"1.*.3", "1.*.3", false, "", "1", "*", "3", "", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p));
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_patch_only)
{
	const lexer::parts p {"1.2.*", "1.2.*", false, "", "1", "2", "*", "", ""};

	EXPECT_EQ(semver("1.2.0"), lower_bound(p));
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p));
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_and_patch)
{
	const lexer::parts p {"1.*.*", "1.*.*", false, "", "1", "*", "*", "", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_and_minor_and_patch)
{
	const lexer::parts p {"*.*.*", "*.*.*", false, "", "*", "*", "*", "", ""};

	EXPECT_EQ(semver::min(), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver::max(), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, tilderange_major_minor_patch_1)
{
	const lexer::parts p {"~1.2.3", "1.2.3", true, "~", "1", "2", "3", "", ""};

	EXPECT_EQ(semver("1.2.3"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, tilderange_major_minor_patch_2)
{
	const lexer::parts p {"~0.2.3", "0.2.3", true, "~", "0", "2", "3", "", ""};

	EXPECT_EQ(semver("0.2.3"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("0.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, tilderange_major_minor_1)
{
	const lexer::parts p {"~1.2", "1.2", false, "~", "1", "2", "", "", ""};

	EXPECT_EQ(semver("1.2.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, tilderange_major_minor_2)
{
	const lexer::parts p {"~0.2", "0.2", false, "~", "0", "2", "", "", ""};

	EXPECT_EQ(semver("0.2.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("0.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, tilderange_major_1)
{
	const lexer::parts p {"~1", "1", false, "~", "1", "", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, tilderange_major_2)
{
	const lexer::parts p {"~0", "0", false, "~", "0", "", "", "", ""};

	EXPECT_EQ(semver("0.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, tilderange_major_minor_patch_prerelease)
{
	const lexer::parts p {"~1.2.3-beta.2", "1.2.3-beta.2", true, "~", "1", "2", "3", "beta.2", ""};

	EXPECT_EQ(semver("1.2.3-beta.2"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_patch)
{
	const lexer::parts p {"^1.2.3", "1.2.3", true, "^", "1", "2", "3", "", ""};

	EXPECT_EQ(semver("1.2.3"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_patch_with_xrange_1)
{
	const lexer::parts p {"^1.2.*", "1.2.*", false, "^", "1", "2", "*", "", ""};

	EXPECT_EQ(semver("1.2.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_patch_with_xrange_2)
{
	const lexer::parts p {"^0.0.*", "0.0.*", false, "^", "0", "0", "*", "", ""};

	EXPECT_EQ(semver("0.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("0.1.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_1)
{
	const lexer::parts p {"^0.0", "0.0", false, "^", "0", "0", "", "", ""};

	EXPECT_EQ(semver("0.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("0.1.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_with_xrange_1)
{
	const lexer::parts p {"^1.*", "1.*", false, "^", "1", "*", "", "", ""};

	EXPECT_EQ(semver("1.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_with_xrange_2)
{
	const lexer::parts p {"^0.*", "0.*", false, "^", "0", "*", "", "", ""};

	EXPECT_EQ(semver("0.0.0"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("1.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major0_minor_patch)
{
	const lexer::parts p {"^0.2.3", "0.2.3", true, "^", "0", "2", "3", "", ""};

	EXPECT_EQ(semver("0.2.3"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("0.3.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major0_minor0_patch)
{
	const lexer::parts p {"^0.0.3", "0.0.3", true, "^", "0", "0", "3", "", ""};

	EXPECT_EQ(semver("0.0.3"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("0.0.4-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_patch_prerelease_1)
{
	const lexer::parts p {"^1.2.3-beta.2", "1.2.3-beta.2", true, "^", "1", "2", "3", "beta.2", ""};

	EXPECT_EQ(semver("1.2.3-beta.2"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("2.0.0-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, caretrange_major_minor_patch_prerelease_2)
{
	const lexer::parts p {"^0.0.3-beta", "0.0.3-beta", true, "^", "0", "0", "3", "beta", ""};

	EXPECT_EQ(semver("0.0.3-beta"), lower_bound(p)) << "token=" << p.token;
	EXPECT_EQ(semver("0.0.4-0"), upper_bound(p)) << "token=" << p.token;
}

TEST_F(test_range_bounds, node_bounds_non_leaf)
{
	const auto node_and = node::create_and(node::create_eq(semver{}), node::create_eq(semver{}));
	const auto node_or = node::create_or(node::create_eq(semver{}), node::create_eq(semver{}));

	EXPECT_FALSE(lower_bound(node_and).ok());
	EXPECT_FALSE(lower_bound(node_or).ok());

	EXPECT_FALSE(upper_bound(node_and).ok());
	EXPECT_FALSE(upper_bound(node_or).ok());
}

TEST_F(test_range_bounds, node_bounds_eq)
{
	const auto n = node::create_eq(semver("1.2.3"));
	const auto expected_min = semver("1.2.3");
	const auto expected_max = expected_min;

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(v_min, v_max);

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_lt)
{
	const auto n = node::create_lt(semver("1.2.3"));
	const auto expected_min = semver::min();
	const auto expected_max = semver("1.2.3-0");

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_le)
{
	const auto n = node::create_le(semver("1.2.3"));
	const auto expected_min = semver::min();
	const auto expected_max = semver("1.2.3");

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_ge)
{
	const auto n = node::create_ge(semver("1.2.3"));
	const auto expected_min = semver("1.2.3");
	const auto expected_max = semver::max();

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_gt)
{
	const auto n = node::create_gt(semver("1.2.3"));
	const auto expected_min = semver("1.2.4");
	const auto expected_max = semver::max();

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_eq_prerelease)
{
	const auto n = node::create_eq(semver("1.2.3-0"));
	const auto expected_min = semver("1.2.3-0");
	const auto expected_max = expected_min;

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(v_min, v_max);

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_lt_prerelease)
{
	const auto n = node::create_lt(semver("1.2.3-0"));
	const auto expected_min = semver::min();
	const auto expected_max = semver("1.2.2");

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_le_prerelease)
{
	const auto n = node::create_le(semver("1.2.3-0"));
	const auto expected_min = semver::min();
	const auto expected_max = semver("1.2.3-0");

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_ge_prerelease)
{
	const auto n = node::create_ge(semver("1.2.3-0"));
	const auto expected_min = semver("1.2.3-0");
	const auto expected_max = semver::max();

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}

TEST_F(test_range_bounds, node_bounds_gt_prerelease)
{
	const auto n = node::create_gt(semver("1.2.3-0"));
	const auto expected_min = semver("1.2.3");
	const auto expected_max = semver::max();

	const auto v_min = lower_bound(n);
	const auto v_max = upper_bound(n);

	EXPECT_TRUE(v_min.ok());
	EXPECT_TRUE(v_max.ok());

	EXPECT_EQ(expected_min, v_min);
	EXPECT_EQ(expected_max, v_max);
}
}

