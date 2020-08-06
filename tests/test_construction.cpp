#include <semver/semver.hpp>
#include <gtest/gtest.h>

namespace {

using semver::semver;

class test_construction : public ::testing::Test {};

#define EXPECT_SV_EQ(s, e) EXPECT_EQ(std::string_view(s), (e))

TEST_F(test_construction, plain_1)
{
	const auto v = semver("1.2.3");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, plain_2)
{
	const auto v = semver("0.0.0");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("0", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, with_numeric_build)
{
	const auto v = semver("1.2.3+123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_construction, with_alphanumeric_build)
{
	const auto v = semver("1.2.3+abc123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_construction, with_numeric_prerelease)
{
	const auto v = semver("1.2.3-123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("123", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, with_alphanumeric_prerelease)
{
	const auto v = semver("1.2.3-abc123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("abc123", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, with_numeric_prerelease_numeric_build)
{
	const auto v = semver("1.2.3-456+123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("456", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_construction, with_alphanumeric_prerelease_numeric_build_1)
{
	const auto v = semver("1.2.3-456abc+123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("456abc", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_construction, with_alphanumeric_prerelease_numeric_build_2)
{
	const auto v = semver("1.2.3-abc456+123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("abc456", v.prerelease());
	EXPECT_SV_EQ("123", v.build());
}

TEST_F(test_construction, with_numeric_prerelease_alphanumeric_build_1)
{
	const auto v = semver("1.2.3-456+abc123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("456", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_construction, with_numeric_prerelease_alphanumeric_build_2)
{
	const auto v = semver("1.2.3-456+123abc");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("456", v.prerelease());
	EXPECT_SV_EQ("123abc", v.build());
}

TEST_F(test_construction, with_alphanumeric_prerelease_alphanumeric_build_1)
{
	const auto v = semver("1.2.3-abc456+abc123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("abc456", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_construction, with_alphanumeric_prerelease_alphanumeric_build_2)
{
	const auto v = semver("1.2.3-456abc+abc123");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("456abc", v.prerelease());
	EXPECT_SV_EQ("abc123", v.build());
}

TEST_F(test_construction, with_alphanumeric_prerelease_alphanumeric_build_3)
{
	const auto v = semver("1.2.3-abc456+123abc");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("abc456", v.prerelease());
	EXPECT_SV_EQ("123abc", v.build());
}

TEST_F(test_construction, with_alphanumeric_prerelease_alphanumeric_build_4)
{
	const auto v = semver("1.2.3-456abc+123abc");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("2", v.minor());
	EXPECT_SV_EQ("3", v.patch());
	EXPECT_SV_EQ("456abc", v.prerelease());
	EXPECT_SV_EQ("123abc", v.build());
}

TEST_F(test_construction, example_from_semver_org_1)
{
	const auto v = semver("1.0.0-alpha");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("alpha", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_2)
{
	const auto v = semver("1.0.0-alpha.1");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("alpha.1", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_3)
{
	const auto v = semver("1.0.0-alpha.beta");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("alpha.beta", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_4)
{
	const auto v = semver("1.0.0-beta.2");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("beta.2", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_5)
{
	const auto v = semver("1.0.0-beta.11");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("beta.11", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_6)
{
	const auto v = semver("1.0.0-rc.1");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("rc.1", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_7)
{
	const auto v = semver("1.0.0-alpha+001");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("alpha", v.prerelease());
	EXPECT_SV_EQ("001", v.build());
}

TEST_F(test_construction, example_from_semver_org_8)
{
	const auto v = semver("1.0.0+20130313144700");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("20130313144700", v.build());
}

TEST_F(test_construction, example_from_semver_org_9)
{
	const auto v = semver("1.0.0-beta+exp.sha.5114f85");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("beta", v.prerelease());
	EXPECT_SV_EQ("exp.sha.5114f85", v.build());
}

TEST_F(test_construction, example_from_semver_org_10)
{
	const auto v = semver("1.0.0+21AF26D3--117B344092BD");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("", v.prerelease());
	EXPECT_SV_EQ("21AF26D3--117B344092BD", v.build());
}

TEST_F(test_construction, example_from_semver_org_11)
{
	const auto v = semver("1.0.0-x.7.z.92");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("x.7.z.92", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_12)
{
	const auto v = semver("1.0.0-0.3.7");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("0.3.7", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}

TEST_F(test_construction, example_from_semver_org_13)
{
	const auto v = semver("1.0.0-x-y-z.-");

	EXPECT_TRUE(v.ok());
	EXPECT_SV_EQ("1", v.major());
	EXPECT_SV_EQ("0", v.minor());
	EXPECT_SV_EQ("0", v.patch());
	EXPECT_SV_EQ("x-y-z.-", v.prerelease());
	EXPECT_SV_EQ("", v.build());
}
}

