#include <semver/range.hpp>
#include <gtest/gtest.h>

namespace
{
using semver::range;

class test_range_construction : public ::testing::Test {};

TEST_F(test_range_construction, single_version_equal_implicit)
{
	const auto r = range("1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, single_version_equal)
{
	const auto r = range("=1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, single_version_less)
{
	const auto r = range("<1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, single_version_less_or_equal)
{
	const auto r = range("<=1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, single_version_greater)
{
	const auto r = range(">1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, single_version_greater_or_equal)
{
	const auto r = range(">=1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, set_1)
{
	const auto r = range(">=1.2.3 <2.0.0");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, set_2)
{
	const auto r = range(">1.2.3 <=2.0.0");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, set_3)
{
	const auto r = range(">1.2.3 <2.0.0 || 3.0.0");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, set_4)
{
	const auto r = range(">1.2.3 <2.0.0 || =3.0.0");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, set_5)
{
	const auto r = range(">1.2.3 <2.0.0 || =3.0.0 || >5.0");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, empty)
{
	const auto r = range("");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, hyphen_range_full)
{
	const auto r = range("1.2.3 - 2.3.4");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, hyphen_range_partial_1)
{
	const auto r = range("1.2 - 2.3.4");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, hyphen_range_partial_2)
{
	const auto r = range("1.2.3 - 2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, hyphen_range_partial_3)
{
	const auto r = range("1.2.3 - 2");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, x_ranges_1)
{
	const auto r = range("*");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, x_ranges_2)
{
	const auto r = range("1.x");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, x_ranges_3)
{
	const auto r = range("1.2.x");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, tilde_ranges_1)
{
	const auto r = range("~1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, tilde_ranges_2)
{
	const auto r = range("~1.2");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, tilde_ranges_3)
{
	const auto r = range("~1");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, tilde_ranges_4)
{
	const auto r = range("~0.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, tilde_ranges_5)
{
	const auto r = range("~0.2");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, tilde_ranges_6)
{
	const auto r = range("~0");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, tilde_ranges_7)
{
	const auto r = range("~1.2.3-beta.2");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_ranges_1)
{
	const auto r = range("^1.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_ranges_2)
{
	const auto r = range("^0.2.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_ranges_3)
{
	const auto r = range("^0.0.3");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_ranges_4)
{
	const auto r = range("^1.2.3-beta.2");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_ranges_5)
{
	const auto r = range("^0.0.3-beta");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_x_ranges_1)
{
	const auto r = range("^1.2.x");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_x_ranges_2)
{
	const auto r = range("^0.0.x");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_x_ranges_3)
{
	const auto r = range("^0.0");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_x_ranges_4)
{
	const auto r = range("^1.x");

	ASSERT_TRUE(r.ok());
}

TEST_F(test_range_construction, caret_x_ranges_5)
{
	const auto r = range("^0.x");

	ASSERT_TRUE(r.ok());
}
}

