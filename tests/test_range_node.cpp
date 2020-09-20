#include "detail/range_node.hpp"
#include <gtest/gtest.h>

namespace
{
using node = semver::detail::node;
using semver = semver::semver;

class test_range_node : public ::testing::Test {};

TEST_F(test_range_node, version)
{
	const auto v = semver("2.4.0-0");

	ASSERT_TRUE(v.ok());

	const auto n = node::create_lt(v);

	EXPECT_EQ(node::type::op_lt, n.get_type());
	EXPECT_EQ(v, n.get_version());
}

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
	const auto n = node::create_lt(semver("1.2.3"));

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
	const auto n = node::create_le(semver("1.2.3"));

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
	const auto n = node::create_gt(semver("1.2.3"));

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
	const auto n = node::create_ge(semver("1.2.3"));

	EXPECT_TRUE(n.eval(semver("1.2.3")));
	EXPECT_FALSE(n.eval(semver("1.2.2")));
	EXPECT_TRUE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.1.3")));
	EXPECT_TRUE(n.eval(semver("1.3.3")));
	EXPECT_FALSE(n.eval(semver("0.2.3")));
	EXPECT_TRUE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, or_1)
{
	const auto n = node::create_or(
		node::create_eq(semver("1.2.3")),
		node::create_eq(semver("3.2.1"))
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

TEST_F(test_range_node, or_2)
{
	const auto n = node::create_or(
		node::create_lt(semver("1.3.0")),
		node::create_eq(semver("2.3.4"))
		);

	EXPECT_TRUE(n.eval(semver("2.3.4")));
	EXPECT_TRUE(n.eval(semver("1.2.3")));
	EXPECT_TRUE(n.eval(semver("1.2.2")));
	EXPECT_TRUE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.3.0")));
	EXPECT_FALSE(n.eval(semver("1.3.3")));
	EXPECT_TRUE(n.eval(semver("0.2.3")));
	EXPECT_FALSE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, and_1)
{
	const auto n = node::create_and(
		node::create_gt(semver("1.2.3")),
		node::create_lt(semver("1.3.0"))
		);

	EXPECT_FALSE(n.eval(semver("1.2.3")));
	EXPECT_FALSE(n.eval(semver("3.2.1")));
	EXPECT_FALSE(n.eval(semver("1.2.2")));
	EXPECT_TRUE(n.eval(semver("1.2.4")));
	EXPECT_FALSE(n.eval(semver("1.1.3")));
	EXPECT_FALSE(n.eval(semver("1.3.3")));
	EXPECT_FALSE(n.eval(semver("0.2.3")));
	EXPECT_FALSE(n.eval(semver("2.2.3")));
}

TEST_F(test_range_node, equal_leafs_1)
{
	const auto n1 = node::create_eq(semver("1.2.3"));
	const auto n2 = node::create_eq(semver("1.2.3"));

	EXPECT_EQ(n1, n2);
}

TEST_F(test_range_node, equal_leafs_2)
{
	const auto n1 = node::create_eq(semver("1.2.3"));
	const auto n2 = node::create_lt(semver("1.2.3"));

	EXPECT_NE(n1, n2);
}

TEST_F(test_range_node, equal_leafs_3)
{
	const auto n1 = node::create_eq(semver("1.2.3"));
	const auto n2 = node::create_le(semver("1.2.3"));

	EXPECT_NE(n1, n2);
}

TEST_F(test_range_node, equal_leafs_4)
{
	const auto n1 = node::create_eq(semver("1.2.3"));
	const auto n2 = node::create_gt(semver("1.2.3"));

	EXPECT_NE(n1, n2);
}

TEST_F(test_range_node, equal_leafs_5)
{
	const auto n1 = node::create_eq(semver("1.2.3"));
	const auto n2 = node::create_ge(semver("1.2.3"));

	EXPECT_NE(n1, n2);
}

TEST_F(test_range_node, equal_leaf_nonleaf)
{
	const auto n1 = node::create_eq(semver("1.2.3"));
	const auto n2 = node::create_and(
		node::create_gt(semver("1.0.0")),
		node::create_lt(semver("2.0.0")));

	EXPECT_NE(n1, n2);
}

TEST_F(test_range_node, equal_nonleafs)
{
	const auto n1 = node::create_and(
		node::create_gt(semver("1.0.0")),
		node::create_lt(semver("2.0.0")));
	const auto n2 = node::create_and(
		node::create_gt(semver("1.0.0")),
		node::create_lt(semver("2.0.0")));

	EXPECT_EQ(n1, n2);
}

TEST_F(test_range_node, less_leafs_1)
{
	const auto n1 = node::create_eq(semver("1.2.0"));
	const auto n2 = node::create_eq(semver("1.5.0"));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_leafs_2)
{
	const auto n1 = node::create_eq(semver("1.2.0"));
	const auto n2 = node::create_lt(semver("1.5.0"));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_leafs_3)
{
	const auto n1 = node::create_eq(semver("1.5.0"));
	const auto n2 = node::create_eq(semver("1.5.0"));

	EXPECT_FALSE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_leaf_nonleaf)
{
	const auto n1 = node::create_eq(semver("1.2.3"));
	const auto n2 = node::create_and(
		node::create_gt(semver("1.0.0")),
		node::create_lt(semver("2.0.0")));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_same)
{
	const auto n1 = node::create_and(
		node::create_gt(semver("1.0.0")),
		node::create_lt(semver("2.0.0")));
	const auto n2 = node::create_and(
		node::create_gt(semver("1.0.0")),
		node::create_lt(semver("2.0.0")));

	EXPECT_FALSE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_diff_1)
{
	const auto n1 = node::create_and(
		node::create_gt(semver("1.0.0")),
		node::create_lt(semver("3.0.0")));
	const auto n2 = node::create_and(
		node::create_gt(semver("2.0.0")),
		node::create_lt(semver("3.0.0")));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_diff_2)
{
	const auto n1 = node::create_and(
		node::create_gt(semver("2.0.0")),
		node::create_lt(semver("2.5.0")));
	const auto n2 = node::create_and(
		node::create_gt(semver("2.0.0")),
		node::create_lt(semver("3.0.0")));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_diff_3)
{
	const auto n1 = node::create_and(
		node::create_gt(semver("2.5.0")),
		node::create_lt(semver("3.0.0")));
	const auto n2 = node::create_and(
		node::create_gt(semver("2.0.0")),
		node::create_lt(semver("3.0.0")));

	EXPECT_FALSE(node::less(n1, n2));
	EXPECT_TRUE(node::less(n2, n1));
}
}

