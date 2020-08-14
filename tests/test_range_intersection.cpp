#include <semver/range.hpp>
#include <gtest/gtest.h>

namespace
{
using semver::range;

class test_range_intersection : public ::testing::Test {};

TEST_F(test_range_intersection, no_intersectoin)
{
	const auto r1 = range(">=1.2.3 <2.0.0");
	const auto r2 = range(">2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_FALSE(intersect(r1, r2));
}

TEST_F(test_range_intersection, equality)
{
	const auto r1 = range(">=1.2.3 <2.0.0");
	const auto r2 = range(">=1.2.3 <2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_TRUE(intersect(r1, r2));
}

TEST_F(test_range_intersection, lower_bound)
{
	const auto r1 = range(">=2.0.0 <3.0.0");
	const auto r2 = range(">=1.2.3 <=2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_TRUE(intersect(r1, r2));
}

TEST_F(test_range_intersection, lower_bound_overlap)
{
	const auto r1 = range(">=2.0.0 <3.0.0");
	const auto r2 = range(">=1.2.3 <=2.1.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_TRUE(intersect(r1, r2));
}

TEST_F(test_range_intersection, upper_bound)
{
	const auto r1 = range(">=2.0.0 <3.0.0");
	const auto r2 = range(">=3.0.0 <=4.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_TRUE(intersect(r1, r2));
}

TEST_F(test_range_intersection, upper_bound_overlap)
{
	const auto r1 = range(">=2.0.0 <3.1.0");
	const auto r2 = range(">=3.0.0 <4.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_TRUE(intersect(r1, r2));
}
}

