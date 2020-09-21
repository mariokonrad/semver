#include <semver/semver.hpp>
#include <gtest/gtest.h>

namespace {

using semver::semver;

#define EXPECT_SV(s, e) EXPECT_EQ(std::string_view(s), (e))

class test_semver_construction_loose : public ::testing::Test {};

TEST_F(test_semver_construction_loose, conatining_spaces_in_front)
{
	const auto v = semver("  1.2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, conatining_spaces_in_between_1)
{
	const auto v = semver("1. 2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, conatining_spaces_in_between_2)
{
	const auto v = semver("1.2 .3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, conatining_spaces_in_between_3)
{
	const auto v = semver("1 . 2 . 3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, conatining_spaces_at_end)
{
	const auto v = semver("1.2.3  ", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, with_prerelease_conatining_spaces_in_between_1)
{
	const auto v = semver("1.2.3- pre1", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("pre1", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, with_prerelease_conatining_spaces_in_between_2)
{
	const auto v = semver("1.2.3-pre 1", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("pre1", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, with_prerelease_conatining_spaces_in_between_3)
{
	const auto v = semver("1.2.3- pre1", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("pre1", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, with_prerelease_conatining_spaces_in_between_4)
{
	const auto v = semver("1.2.3 -pre1", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("pre1", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, construction_loose_with_prefixes_1)
{
	const auto v = semver("v1.2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, construction_loose_with_prefixes_2)
{
	const auto v = semver(">1.2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, construction_loose_with_prefixes_3)
{
	const auto v = semver(">=1.2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, construction_loose_with_prefixes_4)
{
	const auto v = semver("==1.2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, construction_loose_with_prefixes_5)
{
	const auto v = semver("~1.2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}

TEST_F(test_semver_construction_loose, construction_loose_with_prefixes_6)
{
	const auto v = semver("^1.2.3", true);

	ASSERT_TRUE(v.ok());
	EXPECT_EQ(1u, v.major());
	EXPECT_EQ(2u, v.minor());
	EXPECT_EQ(3u, v.patch());
	EXPECT_SV("", v.prerelease());
	EXPECT_SV("", v.build());
}
}

