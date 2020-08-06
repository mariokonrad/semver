#include <semver/semver.hpp>
#include <gtest/gtest.h>

namespace {

using semver::semver;

class test_comparison : public ::testing::Test {};

TEST_F(test_comparison, example_from_semver_org_1)
{
	const auto v1 = semver("1.0.0");
	const auto v2 = semver("2.0.0");
	const auto v3 = semver("2.1.0");
	const auto v4 = semver("2.1.1");

	EXPECT_TRUE(v1 < v2);
	EXPECT_TRUE(v2 < v3);
	EXPECT_TRUE(v3 < v4);
}

TEST_F(test_comparison, example_from_semver_org_2)
{
	const auto v1 = semver("1.0.0-alpha");
	const auto v2 = semver("1.0.0");

	EXPECT_TRUE(v1 < v2);
}

TEST_F(test_comparison, example_from_semver_org_3)
{
	auto v1 = semver("1.0.0-alpha");
	auto v2 = semver("1.0.0-alpha.1");
	auto v3 = semver("1.0.0-alpha.beta");
	auto v4 = semver("1.0.0-beta");
	auto v5 = semver("1.0.0-beta.2");
	auto v6 = semver("1.0.0-beta.11");
	auto v7 = semver("1.0.0-rc.1");
	auto v8 = semver("1.0.0");

	EXPECT_TRUE(v1 < v2);
	EXPECT_TRUE(v2 < v3);
	EXPECT_TRUE(v3 < v4);
	EXPECT_TRUE(v4 < v5);
	EXPECT_TRUE(v5 < v6);
	EXPECT_TRUE(v6 < v7);
	EXPECT_TRUE(v7 < v8);
}
}

