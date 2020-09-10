#include <semver/semver.hpp>
#include <gtest/gtest.h>

namespace {

using ::semver::v1::semver;

class test_semver_construction : public ::testing::Test {};

TEST_F(test_semver_construction, numerical_major_minor_patch)
{
	const auto v = semver(1, 2, 3);

	EXPECT_TRUE(v.ok());
	EXPECT_EQ(1, v.major());
	EXPECT_EQ(2, v.minor());
	EXPECT_EQ(3, v.patch());
	EXPECT_STREQ("1.2.3", to_string(v).c_str());
}

TEST_F(test_semver_construction, numerical_empty_prerelease)
{
	const auto v = semver(1, 2, 3, "");

	EXPECT_TRUE(v.ok());
	EXPECT_EQ(1, v.major());
	EXPECT_EQ(2, v.minor());
	EXPECT_EQ(3, v.patch());
	EXPECT_STREQ("", v.prerelease().c_str());
	EXPECT_STREQ("1.2.3", to_string(v).c_str());
}

TEST_F(test_semver_construction, numerical_prerelease)
{
	const auto v = semver(1, 2, 3, "beta.2-1");

	EXPECT_TRUE(v.ok());
	EXPECT_EQ(1, v.major());
	EXPECT_EQ(2, v.minor());
	EXPECT_EQ(3, v.patch());
	EXPECT_STREQ("beta.2-1", v.prerelease().c_str());
	EXPECT_STREQ("1.2.3-beta.2-1", to_string(v).c_str());
}

TEST_F(test_semver_construction, numerical_invalid_prerelease)
{
	const auto v = semver(1, 2, 3, "beta+2");

	EXPECT_FALSE(v.ok());
}
}

