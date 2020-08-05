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
}

