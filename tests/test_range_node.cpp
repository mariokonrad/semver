#include <semver/range.hpp>
#include <gtest/gtest.h>

namespace
{
using node = semver::detail::node;
using semver = semver::semver;

class test_range_node : public ::testing::Test {};

TEST_F(test_range_node, eq)
{
	const auto n = node::create_eq(semver("1.2.3"));

	EXPECT_TRUE(n.eval(semver("1.2.3")));
	EXPECT_FALSE(n.eval(semver("1.2.2")));
	EXPECT_FALSE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.1.3")));
	EXPECT_FALSE(n.eval(semver("1.3.3")));
	EXPECT_FALSE(n.eval(semver("0.2.3")));
	EXPECT_FALSE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, lt)
{
	const auto n = node::create_eq(semver("1.2.3"));

	EXPECT_FALSE(n.eval(semver("1.2.3")));
	EXPECT_TRUE(n.eval(semver("1.2.2")));
	EXPECT_FALSE(n.eval(semver("1.2.4")));
	EXPECT_TRUE(n.eval(semver("1.1.3")));
	EXPECT_FALSE(n.eval(semver("1.3.3")));
	EXPECT_TRUE(n.eval(semver("0.2.3")));
	EXPECT_FALSE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, le)
{
	const auto n = node::create_eq(semver("1.2.3"));

	EXPECT_TRUE(n.eval(semver("1.2.3")));
	EXPECT_TRUE(n.eval(semver("1.2.2")));
	EXPECT_FALSE(n.eval(semver("1.2.4")));
	EXPECT_TRUE(n.eval(semver("1.1.3")));
	EXPECT_FALSE(n.eval(semver("1.3.3")));
	EXPECT_TRUE(n.eval(semver("0.2.3")));
	EXPECT_FALSE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, gt)
{
	const auto n = node::create_eq(semver("1.2.3"));

	EXPECT_FALSE(n.eval(semver("1.2.3")));
	EXPECT_FALSE(n.eval(semver("1.2.2")));
	EXPECT_TRUE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.1.3")));
	EXPECT_TRUE(n.eval(semver("1.3.3")));
	EXPECT_FALSE(n.eval(semver("0.2.3")));
	EXPECT_TRUE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, ge)
{
	const auto n = node::create_eq(semver("1.2.3"));

	EXPECT_TRUE(n.eval(semver("1.2.3")));
	EXPECT_FALSE(n.eval(semver("1.2.2")));
	EXPECT_TRUE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.1.3")));
	EXPECT_TRUE(n.eval(semver("1.3.3")));
	EXPECT_FALSE(n.eval(semver("0.2.3")));
	EXPECT_TRUE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, or)
{
	const auto n = node::create_or(
		std::make_unique<node>(node::create_eq(semver("1.2.3"))),
		std::make_unique<node>(node::create_eq(semver("3.2.1")))
		);

	EXPECT_TRUE(n.eval(semver("1.2.3")));
	EXPECT_TRUE(n.eval(semver("3.2.1")));
	EXPECT_FALSE(n.eval(semver("1.2.2")));
	EXPECT_FALSE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.1.3")));
	EXPECT_FALSE(n.eval(semver("1.3.3")));
	EXPECT_FALSE(n.eval(semver("0.2.3")));
	EXPECT_FALSE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, and)
{
	const auto n = node::create_and(
		std::make_unique<node>(node::create_gt(semver("1.2.3"))),
		std::make_unique<node>(node::create_lt(semver("1.3.0")))
		);

	EXPECT_FALSE(n.eval(semver("1.2.3")));
	EXPECT_FALSE(n.eval(semver("3.2.1")));
	EXPECT_TRUE(n.eval(semver("1.2.2")));
	EXPECT_TRUE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.1.3")));
	EXPECT_TRUE(n.eval(semver("1.3.3")));
	EXPECT_FALSE(n.eval(semver("0.2.3")));
	EXPECT_FALSE(n.eval(semver("2.2.3")));
}
}

