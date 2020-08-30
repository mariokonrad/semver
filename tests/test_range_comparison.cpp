#include <semver/range.hpp>
#include <gtest/gtest.h>

namespace
{
using semver::range;

class test_range_comparison : public ::testing::Test {};

TEST_F(test_range_comparison, equality_of_same_strings_1)
{
	const auto r1 = range(">=1.2.3 <2.0.0");
	const auto r2 = range(">=1.2.3 <2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_same_strings_2)
{
	const auto r1 = range(">1.2.3 <=2.0.0");
	const auto r2 = range(">1.2.3 <=2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_hyphen_and_explicit_1)
{
	const auto r1 = range("1.2.3 - 2.3.4");
	const auto r2 = range(">=1.2.3 <=2.3.4");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_hyphen_and_explicit_2)
{
	const auto r1 = range("1.2 - 2.3.4");
	const auto r2 = range(">=1.2.0 <=2.3.4");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_hyphen_and_explicit_3)
{
	const auto r1 = range("1.2.3 - 2.3");
	const auto r2 = range(">=1.2.3 <2.4.0-0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_hyphen_and_explicit_4)
{
	const auto r1 = range("1.2.3 - 2");
	const auto r2 = range(">=1.2.3 <3.0.0-0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_xrange_and_explicit_1)
{
	const auto r1 = range("*");
	const auto r2 = range(">=0.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_xrange_and_explicit_2)
{
	const auto r1 = range("1.x");
	const auto r2 = range(">=1.0.0 <2.0.0-0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_xrange_and_explicit_3)
{
	const auto r1 = range("1.2.x");
	const auto r2 = range(">=1.2.0 <1.3.0-0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_xrange_and_explicit_4)
{
	const auto r1 = range("");
	const auto r2 = range("*");
	const auto r3 = range(">=0.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());
	ASSERT_TRUE(r3.ok());

	EXPECT_EQ(r1, r2);
	EXPECT_EQ(r1, r3);
	EXPECT_EQ(r2, r3);
}

TEST_F(test_range_comparison, equality_of_xrange_and_explicit_5)
{
	const auto r1 = range("1");
	const auto r2 = range("1.x.x");
	const auto r3 = range(">=1.0.0 <2.0.0-0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());
	ASSERT_TRUE(r3.ok());

	EXPECT_EQ(r1, r2);
	EXPECT_EQ(r1, r3);
	EXPECT_EQ(r2, r3);
}

TEST_F(test_range_comparison, equality_of_xrange_and_explicit_6)
{
	const auto r1 = range("1.2");
	const auto r2 = range("1.2.x");
	const auto r3 = range(">=1.2.0 <1.3.0-0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());
	ASSERT_TRUE(r3.ok());

	EXPECT_EQ(r1, r2);
	EXPECT_EQ(r1, r3);
	EXPECT_EQ(r2, r3);
}

TEST_F(test_range_comparison, equality_of_tilde_range_and_explicit_1)
{
	const auto r1 = range("~1.2.3");
	const auto r2 = range(">=1.2.3 <1.3.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_tilde_range_and_explicit_2)
{
	const auto r1 = range("~1.2");
	const auto r2 = range(">=1.2.0 <1.3.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_tilde_range_and_explicit_3)
{
	const auto r1 = range("~1");
	const auto r2 = range(">=1.0.0 <2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_tilde_range_and_explicit_4)
{
	const auto r1 = range("~0.2.3");
	const auto r2 = range(">=0.2.3 <0.3.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_tilde_range_and_explicit_5)
{
	const auto r1 = range("~0.2");
	const auto r2 = range(">=0.2.0 <0.3.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_tilde_range_and_explicit_6)
{
	const auto r1 = range("~0");
	const auto r2 = range(">=0.0.0 <1.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_tilde_range_and_explicit_7)
{
	const auto r1 = range("~1.2.3-beta.2");
	const auto r2 = range(">=1.2.3-beta.2 <1.3.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_1)
{
	const auto r1 = range("^1.2.3");
	const auto r2 = range(">=1.2.3 <2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_2)
{
	const auto r1 = range("^0.2.3");
	const auto r2 = range(">=0.2.3 <0.3.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_3)
{
	const auto r1 = range("^0.0.3");
	const auto r2 = range(">=0.0.3 <0.0.4");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_4)
{
	const auto r1 = range("^1.2.3-beta.2");
	const auto r2 = range(">=1.2.3-beta.2 <2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_5)
{
	const auto r1 = range("^0.0.3-beta");
	const auto r2 = range(">=0.0.3-beta <0.0.4");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_6)
{
	const auto r1 = range("^1.2.x");
	const auto r2 = range(">=1.2.0 <2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_7)
{
	const auto r1 = range("^0.0.x");
	const auto r2 = range(">=0.0.0 <0.1.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_8)
{
	const auto r1 = range("^0.0");
	const auto r2 = range(">=0.0.0 <0.1.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_9)
{
	const auto r1 = range("^1.x");
	const auto r2 = range(">=1.0.0 <2.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}

TEST_F(test_range_comparison, equality_of_caret_range_and_explicit_10)
{
	const auto r1 = range("^0.x");
	const auto r2 = range(">=0.0.0 <1.0.0");

	ASSERT_TRUE(r1.ok());
	ASSERT_TRUE(r2.ok());

	EXPECT_EQ(r1, r2);
}
}

