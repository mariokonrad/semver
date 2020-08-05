#include <semver/semver.hpp>
#include <gtest/gtest.h>

namespace {

using semver::semver;

class test_construction : public ::testing::Test {};


TEST_F(test_construction, plain_1)
{
	const auto v = semver("1.2.3");

	EXPECT_TRUE(v.ok());
	EXPECT_EQ(std::string_view{"1"}, v.major());
	EXPECT_EQ(std::string_view{"2"}, v.minor());
	EXPECT_EQ(std::string_view{"3"}, v.patch());
	EXPECT_EQ(std::string_view{""}, v.build());
	EXPECT_EQ(std::string_view{""}, v.prerelease());
}

TEST_F(test_construction, plain_2)
{
	const auto v = semver("0.0.0");

	EXPECT_TRUE(v.ok());
	EXPECT_EQ(std::string_view{"0"}, v.major());
	EXPECT_EQ(std::string_view{"0"}, v.minor());
	EXPECT_EQ(std::string_view{"0"}, v.patch());
	EXPECT_EQ(std::string_view{""}, v.build());
	EXPECT_EQ(std::string_view{""}, v.prerelease());
}

TEST_F(test_construction, with_numeric_build)
{
	const auto v = semver("1.2.3+123");

	EXPECT_TRUE(v.ok());
	EXPECT_EQ(std::string_view{"1"}, v.major());
	EXPECT_EQ(std::string_view{"2"}, v.minor());
	EXPECT_EQ(std::string_view{"3"}, v.patch());
	EXPECT_EQ(std::string_view{"123"}, v.build());
	EXPECT_EQ(std::string_view{""}, v.prerelease());
}

TEST_F(test_construction, with_alphanumeric_build)
{
	const auto v = semver("1.2.3+abc123");

	EXPECT_TRUE(v.ok());
	EXPECT_EQ(std::string_view{"1"}, v.major());
	EXPECT_EQ(std::string_view{"2"}, v.minor());
	EXPECT_EQ(std::string_view{"3"}, v.patch());
	EXPECT_EQ(std::string_view{"abc123"}, v.build());
	EXPECT_EQ(std::string_view{""}, v.prerelease());
}
}

