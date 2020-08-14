#include <semver/semver.hpp>
#include <gtest/gtest.h>

namespace {

using semver::semver;

class test_semver_construction : public ::testing::Test {};

#define EXPECT_SV_EQ(s, e) EXPECT_EQ(std::string_view(s), (e))

TEST_F(test_semver_construction, empty)
{
	const auto v = semver("");

	ASSERT_FALSE(v.ok());
}

TEST_F(test_semver_construction, plain_1)
{
	const auto v = semver("1.2.3");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, plain_2)
{
	const auto v = semver("0.0.0");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(0u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, with_numeric_build)
{
	const auto v = semver("1.2.3+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_build)
{
	const auto v = semver("1.2.3+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_semver_construction, with_numeric_prerelease)
{
	const auto v = semver("1.2.3-123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("123", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_prerelease)
{
	const auto v = semver("1.2.3-abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("abc123", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, with_numeric_prerelease_numeric_build)
{
	const auto v = semver("1.2.3-456+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("456", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_prerelease_numeric_build_1)
{
	const auto v = semver("1.2.3-456abc+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("456abc", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_prerelease_numeric_build_2)
{
	const auto v = semver("1.2.3-abc456+123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("abc456", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_semver_construction, with_numeric_prerelease_alphanumeric_build_1)
{
	const auto v = semver("1.2.3-456+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("456", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_semver_construction, with_numeric_prerelease_alphanumeric_build_2)
{
	const auto v = semver("1.2.3-456+123abc");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("456", v.prerelease());
	EXPECT_SV_EQ("123abc", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_prerelease_alphanumeric_build_1)
{
	const auto v = semver("1.2.3-abc456+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("abc456", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_prerelease_alphanumeric_build_2)
{
	const auto v = semver("1.2.3-456abc+abc123");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("456abc", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_prerelease_alphanumeric_build_3)
{
	const auto v = semver("1.2.3-abc456+123abc");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("abc456", v.prerelease());
	EXPECT_SV_EQ("123abc", v.build());
}

TEST_F(test_semver_construction, with_alphanumeric_prerelease_alphanumeric_build_4)
{
	const auto v = semver("1.2.3-456abc+123abc");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV_EQ("456abc", v.prerelease());
	EXPECT_SV_EQ("123abc", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_1)
{
	const auto v = semver("1.0.0-alpha");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("alpha", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_2)
{
	const auto v = semver("1.0.0-alpha.1");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("alpha.1", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_3)
{
	const auto v = semver("1.0.0-alpha.beta");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("alpha.beta", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_4)
{
	const auto v = semver("1.0.0-beta.2");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("beta.2", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_5)
{
	const auto v = semver("1.0.0-beta.11");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("beta.11", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_6)
{
	const auto v = semver("1.0.0-rc.1");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("rc.1", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_7)
{
	const auto v = semver("1.0.0-alpha+001");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("alpha", v.prerelease());
	EXPECT_SV_EQ("001", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_8)
{
	const auto v = semver("1.0.0+20130313144700");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("20130313144700", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_9)
{
	const auto v = semver("1.0.0-beta+exp.sha.5114f85");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("beta", v.prerelease());
	EXPECT_SV_EQ("exp.sha.5114f85", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_10)
{
	const auto v = semver("1.0.0+21AF26D3--117B344092BD");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("21AF26D3--117B344092BD", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_11)
{
	const auto v = semver("1.0.0-x.7.z.92");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("x.7.z.92", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_12)
{
	const auto v = semver("1.0.0-0.3.7");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("0.3.7", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, example_from_semver_org_13)
{
	const auto v = semver("1.0.0-x-y-z.-");

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(0u, v.minor());
	EXPECT_EQ(0u, v.patch());
	EXPECT_SV_EQ("x-y-z.-", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_semver_construction, failing_multiple_pluses)
{
	const auto v = semver("1.0.0+alpha+beta");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_construction, failing_consecutive_multiple_pluses)
{
	const auto v = semver("1.0.0++beta");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_construction, failing_empty_build)
{
	const auto v = semver("1.0.0+");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_construction, failing_empty_prerelease)
{
	const auto v = semver("1.0.0-");

	EXPECT_FALSE(v.ok());
}

TEST_F(test_semver_construction, failing_empty_prerelease_empty_build)
{
	const auto v = semver("1.0.0-+");

	EXPECT_FALSE(v.ok());
}
}

