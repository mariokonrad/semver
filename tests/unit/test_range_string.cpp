#include <semver/range.hpp>
#include <gtest/gtest.h>

namespace
{
using semver::range;

class test_range_string : public ::testing::Test {};

TEST_F(test_range_string, single)
{
	const auto r = range("1.2.3");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ("=1.2.3", s.c_str());
}

TEST_F(test_range_string, single_lt)
{
	const auto r = range("<1.2.3");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ("<1.2.3", s.c_str());
}

TEST_F(test_range_string, single_le)
{
	const auto r = range("<=1.2.3");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ("<=1.2.3", s.c_str());
}

TEST_F(test_range_string, single_gt)
{
	const auto r = range(">1.2.3");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ(">1.2.3", s.c_str());
}

TEST_F(test_range_string, single_ge)
{
	const auto r = range(">=1.2.3");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ(">=1.2.3", s.c_str());
}

TEST_F(test_range_string, simple_or)
{
	const auto r = range("1.2.3 || 2.3.4");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ("=1.2.3 || =2.3.4", s.c_str());
}

TEST_F(test_range_string, or_with_operators)
{
	const auto r = range(">1.2.3 || <2.3.4");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ(">1.2.3 || <2.3.4", s.c_str());
}

TEST_F(test_range_string, or_with_operators_sorted)
{
	const auto r = range("<2.3.4 || >1.2.3");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ(">1.2.3 || <2.3.4", s.c_str());
}

TEST_F(test_range_string, and_with_operators)
{
	const auto r = range(">1.2.3 <2.3.4");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ(">1.2.3 <2.3.4", s.c_str());
}

TEST_F(test_range_string, and_with_operators_sorted)
{
	const auto r = range("<2.3.4 >1.2.3");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ(">1.2.3 <2.3.4", s.c_str());
}

TEST_F(test_range_string, complex_sorted)
{
	const auto r = range("1.1.1 || <2.3.4 >1.2.3 || 3.2.1 || >5");

	ASSERT_TRUE(r.ok());

	const auto s = to_string(r);

	EXPECT_STREQ("=1.1.1 || =3.2.1 || >5.0.0 || >1.2.3 <2.3.4", s.c_str());
}
}

