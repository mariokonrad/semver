#include <semver/range.hpp>
#include <gtest/gtest.h>

namespace
{
using semver::range;
using semver::semver;

class test_range_query : public ::testing::Test {};

TEST_F(test_range_query, outside_greater)
{
	const auto r = range(">1.2.3");

	ASSERT_TRUE(r.ok());
	EXPECT_TRUE(r.outside(semver("1.0.0")));
	EXPECT_TRUE(r.outside(semver("1.2.0")));
	EXPECT_TRUE(r.outside(semver("1.2.2")));
	EXPECT_TRUE(r.outside(semver("1.2.3")));
	EXPECT_FALSE(r.outside(semver("1.2.4")));
	EXPECT_FALSE(r.outside(semver("1.3.0")));
	EXPECT_FALSE(r.outside(semver("1.9.0")));
	EXPECT_FALSE(r.outside(semver("2.0.0")));
	EXPECT_FALSE(r.outside(semver("2.1.0")));
	EXPECT_FALSE(r.outside(semver("3.0.0")));
	EXPECT_FALSE(r.outside(semver("3.1.0")));
}

TEST_F(test_range_query, outside_less)
{
	const auto r = range("<2.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_FALSE(r.outside(semver("1.0.0")));
	EXPECT_FALSE(r.outside(semver("1.2.0")));
	EXPECT_FALSE(r.outside(semver("1.2.2")));
	EXPECT_FALSE(r.outside(semver("1.2.3")));
	EXPECT_FALSE(r.outside(semver("1.2.4")));
	EXPECT_FALSE(r.outside(semver("1.3.0")));
	EXPECT_FALSE(r.outside(semver("1.9.0")));
	EXPECT_TRUE(r.outside(semver("2.0.0")));
	EXPECT_TRUE(r.outside(semver("2.1.0")));
	EXPECT_TRUE(r.outside(semver("3.0.0")));
	EXPECT_TRUE(r.outside(semver("3.1.0")));
}

TEST_F(test_range_query, outside_inbetween)
{
	const auto r = range(">1.2.3 <2.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_TRUE(r.outside(semver("1.0.0")));
	EXPECT_TRUE(r.outside(semver("1.2.0")));
	EXPECT_TRUE(r.outside(semver("1.2.2")));
	EXPECT_TRUE(r.outside(semver("1.2.3")));
	EXPECT_FALSE(r.outside(semver("1.2.4")));
	EXPECT_FALSE(r.outside(semver("1.3.0")));
	EXPECT_FALSE(r.outside(semver("1.9.0")));
	EXPECT_TRUE(r.outside(semver("2.0.0")));
	EXPECT_TRUE(r.outside(semver("2.1.0")));
	EXPECT_TRUE(r.outside(semver("3.0.0")));
	EXPECT_TRUE(r.outside(semver("3.1.0")));
}

TEST_F(test_range_query, outside_inbetween_with_or)
{
	const auto r = range(">1.2.3 <2.0.0 || 3.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_TRUE(r.outside(semver("1.0.0")));
	EXPECT_TRUE(r.outside(semver("1.2.0")));
	EXPECT_TRUE(r.outside(semver("1.2.2")));
	EXPECT_TRUE(r.outside(semver("1.2.3")));
	EXPECT_FALSE(r.outside(semver("1.2.4")));
	EXPECT_FALSE(r.outside(semver("1.3.0")));
	EXPECT_FALSE(r.outside(semver("1.9.0")));
	EXPECT_TRUE(r.outside(semver("2.0.0")));
	EXPECT_TRUE(r.outside(semver("2.1.0")));
	EXPECT_FALSE(r.outside(semver("3.0.0")));
	EXPECT_TRUE(r.outside(semver("3.1.0")));
}

TEST_F(test_range_query, satisfies_greater)
{
	const auto r = range(">1.2.3");

	ASSERT_TRUE(r.ok());
	EXPECT_FALSE(r.satisfies(semver("1.0.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.2")));
	EXPECT_FALSE(r.satisfies(semver("1.2.3")));
	EXPECT_TRUE(r.satisfies(semver("1.2.4")));
	EXPECT_TRUE(r.satisfies(semver("1.3.0")));
	EXPECT_TRUE(r.satisfies(semver("1.9.0")));
	EXPECT_TRUE(r.satisfies(semver("2.0.0")));
	EXPECT_TRUE(r.satisfies(semver("2.1.0")));
	EXPECT_TRUE(r.satisfies(semver("3.0.0")));
}

TEST_F(test_range_query, satisfies_less)
{
	const auto r = range("<2.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_TRUE(r.satisfies(semver("1.0.0")));
	EXPECT_TRUE(r.satisfies(semver("1.2.0")));
	EXPECT_TRUE(r.satisfies(semver("1.2.2")));
	EXPECT_TRUE(r.satisfies(semver("1.2.3")));
	EXPECT_TRUE(r.satisfies(semver("1.2.4")));
	EXPECT_TRUE(r.satisfies(semver("1.3.0")));
	EXPECT_TRUE(r.satisfies(semver("1.9.0")));
	EXPECT_FALSE(r.satisfies(semver("2.0.0")));
	EXPECT_FALSE(r.satisfies(semver("2.1.0")));
	EXPECT_FALSE(r.satisfies(semver("3.0.0")));
}

TEST_F(test_range_query, satisfies_inbetween)
{
	const auto r = range(">1.2.3 <2.0.0");

	ASSERT_TRUE(r.ok());
	return;
	EXPECT_FALSE(r.satisfies(semver("1.0.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.2")));
	EXPECT_FALSE(r.satisfies(semver("1.2.3")));
	EXPECT_TRUE(r.satisfies(semver("1.2.4")));
	EXPECT_TRUE(r.satisfies(semver("1.3.0")));
	EXPECT_TRUE(r.satisfies(semver("1.9.0")));
	EXPECT_FALSE(r.satisfies(semver("2.0.0")));
	EXPECT_FALSE(r.satisfies(semver("2.1.0")));
	EXPECT_FALSE(r.satisfies(semver("3.0.0")));
}

TEST_F(test_range_query, satisfies_or)
{
	const auto r = range("1.2.3 || 2.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_FALSE(r.satisfies(semver("1.0.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.2")));
	EXPECT_TRUE(r.satisfies(semver("1.2.3")));
	EXPECT_FALSE(r.satisfies(semver("1.2.4")));
	EXPECT_FALSE(r.satisfies(semver("1.3.0")));
	EXPECT_FALSE(r.satisfies(semver("1.9.0")));
	EXPECT_TRUE(r.satisfies(semver("2.0.0")));
	EXPECT_FALSE(r.satisfies(semver("2.1.0")));
	EXPECT_FALSE(r.satisfies(semver("3.0.0")));
}

TEST_F(test_range_query, satisfies_inbetween_with_or)
{
	const auto r = range(">1.2.3 <2.0.0 || 3.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_FALSE(r.satisfies(semver("1.0.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.2")));
	EXPECT_FALSE(r.satisfies(semver("1.2.3")));
	EXPECT_TRUE(r.satisfies(semver("1.2.4")));
	EXPECT_TRUE(r.satisfies(semver("1.3.0")));
	EXPECT_TRUE(r.satisfies(semver("1.9.0")));
	EXPECT_FALSE(r.satisfies(semver("2.0.0")));
	EXPECT_FALSE(r.satisfies(semver("2.1.0")));
	EXPECT_TRUE(r.satisfies(semver("3.0.0")));
}

TEST_F(test_range_query, satisfies_hyphen)
{
	const auto r = range("1.2.3 - 2.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_FALSE(r.satisfies(semver("1.0.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.2")));
	EXPECT_TRUE(r.satisfies(semver("1.2.3")));
	EXPECT_TRUE(r.satisfies(semver("1.2.4")));
	EXPECT_TRUE(r.satisfies(semver("1.3.0")));
	EXPECT_TRUE(r.satisfies(semver("1.9.0")));
	EXPECT_TRUE(r.satisfies(semver("2.0.0")));
	EXPECT_FALSE(r.satisfies(semver("2.1.0")));
	EXPECT_FALSE(r.satisfies(semver("3.0.0")));
}

TEST_F(test_range_query, satisfies_hyphen_with_or)
{
	const auto r = range("1.2.0 || 1.2.3 - 2.0.0");

	ASSERT_TRUE(r.ok());
	EXPECT_FALSE(r.satisfies(semver("1.0.0")));
	EXPECT_TRUE(r.satisfies(semver("1.2.0")));
	EXPECT_FALSE(r.satisfies(semver("1.2.2")));
	EXPECT_TRUE(r.satisfies(semver("1.2.3")));
	EXPECT_TRUE(r.satisfies(semver("1.2.4")));
	EXPECT_TRUE(r.satisfies(semver("1.3.0")));
	EXPECT_TRUE(r.satisfies(semver("1.9.0")));
	EXPECT_TRUE(r.satisfies(semver("2.0.0")));
	EXPECT_FALSE(r.satisfies(semver("2.1.0")));
	EXPECT_FALSE(r.satisfies(semver("3.0.0")));
}

// TODO: satisfies with tilde, caret and xranges

TEST_F(test_range_query, max)
{
	EXPECT_EQ(semver("2.0.0"), range("2.0.0").max());
	EXPECT_EQ(semver("2.0.0"), range("<=2.0.0").max());
	EXPECT_EQ(semver("1.99999.99999"), range("<2.0.0").max());
	EXPECT_EQ(semver("3.0.0"), range("<2.0.0 || 3.0.0").max());
	EXPECT_EQ(semver("99999.99999.99999"), range(">2.0.0").max());
	EXPECT_EQ(semver("99999.99999.99999"), range(">=2.0.0").max());
	EXPECT_EQ(semver("2.0.0"), range(">1.2.3 <=2.0.0").max());
}

TEST_F(test_range_query, min)
{
	EXPECT_EQ(semver("2.0.0"), range("2.0.0").min());
	EXPECT_EQ(semver("0.0.0"), range("<=2.0.0").min());
	EXPECT_EQ(semver("0.0.0"), range("<2.0.0").min());
	EXPECT_EQ(semver("0.0.0"), range("<2.0.0 || 3.0.0").min());
	EXPECT_EQ(semver("2.0.1"), range(">2.0.0").min());
	EXPECT_EQ(semver("2.0.0"), range(">=2.0.0").min());
	EXPECT_EQ(semver("1.2.3"), range(">=1.2.3 <2.0.0").min());
}

TEST_F(test_range_query, max_satisfying)
{
	const auto r = range(">1.2.3 <2.0.0 || 3.0.0");

	EXPECT_EQ(semver("1.11.0"),
		r.max_satisfying({semver("1.2.0"), semver("1.5.0"), semver("1.11.0")}));
	EXPECT_EQ(semver("3.0.0"),
		r.max_satisfying({semver("1.2.0"), semver("3.0.0"), semver("1.11.0")}));
}

TEST_F(test_range_query, min_satisfying)
{
	const auto r = range(">1.2.3 <2.0.0 || 3.0.0");

	EXPECT_EQ(semver("1.5.0"),
		r.min_satisfying({semver("1.2.0"), semver("1.5.0"), semver("1.11.0")}));
	EXPECT_EQ(semver("1.11.0"),
		r.min_satisfying({semver("1.2.0"), semver("3.0.0"), semver("1.11.0")}));
}
}

