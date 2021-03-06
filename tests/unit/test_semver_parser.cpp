#include "detail/semver_parser.hpp"
#include <gtest/gtest.h>

namespace {

using ::semver::v1::detail::semver_parser;

class test_semver_parser : public ::testing::Test {};

#define EXPECT_SV(s, e) EXPECT_EQ(std::string(s), (e))

TEST_F(test_semver_parser, empty)
{
	const auto v = semver_parser("");

	ASSERT_FALSE(v.ok());
}

TEST_F(test_semver_parser, plain_1)
{
	const auto v = semver_parser("1.2.3");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, plain_2)
{
	const auto v = semver_parser("0.0.0");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(0u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, with_numeric_build)
{
	const auto v = semver_parser("1.2.3+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("123", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_build)
{
	const auto v = semver_parser("1.2.3+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("abc123", v.build());
}

TEST_F(test_semver_parser, with_numeric_prerelease)
{
	const auto v = semver_parser("1.2.3-123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("123", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_prerelease)
{
	const auto v = semver_parser("1.2.3-abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("abc123", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, with_numeric_prerelease_numeric_build)
{
	const auto v = semver_parser("1.2.3-456+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("456", v.prerelease());
	EXPECT_SV("123", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_prerelease_numeric_build_1)
{
	const auto v = semver_parser("1.2.3-456abc+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("456abc", v.prerelease());
	EXPECT_SV("123", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_prerelease_numeric_build_2)
{
	const auto v = semver_parser("1.2.3-abc456+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("abc456", v.prerelease());
	EXPECT_SV("123", v.build());
}

TEST_F(test_semver_parser, with_numeric_prerelease_alphanumeric_build_1)
{
	const auto v = semver_parser("1.2.3-456+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("456", v.prerelease());
	EXPECT_SV("abc123", v.build());
}

TEST_F(test_semver_parser, with_numeric_prerelease_alphanumeric_build_2)
{
	const auto v = semver_parser("1.2.3-456+123abc");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("456", v.prerelease());
	EXPECT_SV("123abc", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_prerelease_alphanumeric_build_1)
{
	const auto v = semver_parser("1.2.3-abc456+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("abc456", v.prerelease());
	EXPECT_SV("abc123", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_prerelease_alphanumeric_build_2)
{
	const auto v = semver_parser("1.2.3-456abc+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("456abc", v.prerelease());
	EXPECT_SV("abc123", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_prerelease_alphanumeric_build_3)
{
	const auto v = semver_parser("1.2.3-abc456+123abc");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("abc456", v.prerelease());
	EXPECT_SV("123abc", v.build());
}

TEST_F(test_semver_parser, with_alphanumeric_prerelease_alphanumeric_build_4)
{
	const auto v = semver_parser("1.2.3-456abc+123abc");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("456abc", v.prerelease());
	EXPECT_SV("123abc", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_1)
{
	const auto v = semver_parser("1.0.0-alpha");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("alpha", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_2)
{
	const auto v = semver_parser("1.0.0-alpha.1");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("alpha.1", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_3)
{
	const auto v = semver_parser("1.0.0-alpha.beta");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("alpha.beta", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_4)
{
	const auto v = semver_parser("1.0.0-beta.2");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("beta.2", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_5)
{
	const auto v = semver_parser("1.0.0-beta.11");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("beta.11", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_6)
{
	const auto v = semver_parser("1.0.0-rc.1");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("rc.1", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_7)
{
	const auto v = semver_parser("1.0.0-alpha+001");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("alpha", v.prerelease());
	EXPECT_SV("001", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_8)
{
	const auto v = semver_parser("1.0.0+20130313144700");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("20130313144700", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_9)
{
	const auto v = semver_parser("1.0.0-beta+exp.sha.5114f85");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("beta", v.prerelease());
	EXPECT_SV("exp.sha.5114f85", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_10)
{
	const auto v = semver_parser("1.0.0+21AF26D3--117B344092BD");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("21AF26D3--117B344092BD", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_11)
{
	const auto v = semver_parser("1.0.0-x.7.z.92");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("x.7.z.92", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_12)
{
	const auto v = semver_parser("1.0.0-0.3.7");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("0.3.7", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, example_from_semver_org_13)
{
	const auto v = semver_parser("1.0.0-x-y-z.-");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV("x-y-z.-", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_parser, failing_multiple_pluses)
{
	const auto v = semver_parser("1.0.0+alpha+beta");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_parser, failing_consecutive_multiple_pluses)
{
	const auto v = semver_parser("1.0.0++beta");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_parser, failing_empty_build)
{
	const auto v = semver_parser("1.0.0+");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_parser, failing_empty_prerelease)
{
	const auto v = semver_parser("1.0.0-");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_parser, failing_empty_prerelease_empty_build)
{
	const auto v = semver_parser("1.0.0-+");

	EXPECT_FALSE(v.ok());
}
}

