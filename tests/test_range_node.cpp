#include <semver/range.hpp>
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

TEST_F(test_range_node, or_2)
{
	const auto n = node::create_or(
		std::make_unique<node>(node::create_lt(semver("1.3.0"))),
		std::make_unique<node>(node::create_eq(semver("2.3.4")))
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
		std::make_unique<node>(node::create_gt(semver("1.2.3"))),
		std::make_unique<node>(node::create_lt(semver("1.3.0")))
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
		std::make_unique<node>(node::create_gt(semver("1.0.0"))),
		std::make_unique<node>(node::create_lt(semver("2.0.0"))));

	EXPECT_NE(n1, n2);
}

TEST_F(test_range_node, equal_nonleafs)
{
	const auto n1 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("1.0.0"))),
		std::make_unique<node>(node::create_lt(semver("2.0.0"))));
	const auto n2 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("1.0.0"))),
		std::make_unique<node>(node::create_lt(semver("2.0.0"))));

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
		std::make_unique<node>(node::create_gt(semver("1.0.0"))),
		std::make_unique<node>(node::create_lt(semver("2.0.0"))));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_same)
{
	const auto n1 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("1.0.0"))),
		std::make_unique<node>(node::create_lt(semver("2.0.0"))));
	const auto n2 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("1.0.0"))),
		std::make_unique<node>(node::create_lt(semver("2.0.0"))));

	EXPECT_FALSE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_diff_1)
{
	const auto n1 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("1.0.0"))),
		std::make_unique<node>(node::create_lt(semver("3.0.0"))));
	const auto n2 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("2.0.0"))),
		std::make_unique<node>(node::create_lt(semver("3.0.0"))));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_diff_2)
{
	const auto n1 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("2.0.0"))),
		std::make_unique<node>(node::create_lt(semver("2.5.0"))));
	const auto n2 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("2.0.0"))),
		std::make_unique<node>(node::create_lt(semver("3.0.0"))));

	EXPECT_TRUE(node::less(n1, n2));
	EXPECT_FALSE(node::less(n2, n1));
}

TEST_F(test_range_node, less_nonleafs_diff_3)
{
	const auto n1 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("2.5.0"))),
		std::make_unique<node>(node::create_lt(semver("3.0.0"))));
	const auto n2 = node::create_and(
		std::make_unique<node>(node::create_gt(semver("2.0.0"))),
		std::make_unique<node>(node::create_lt(semver("3.0.0"))));

	EXPECT_FALSE(node::less(n1, n2));
	EXPECT_TRUE(node::less(n2, n1));
}

/*
TEST_F(test_range_node, transform_leaf_only)
{
	auto n = node::create_eq(semver("1.2.3"));
	const std::vector<node::type> expected_types_before = {node::type::op_eq};
	const std::vector<node::type> expected_types_after = {node::type::op_eq};

	std::vector<node::type> visited_types_before;
	std::vector<node::type> visited_types_after;

	n.visit_postfix([&](const node & n) { visited_types_before.push_back(n.get_type()); });
	n.optimize();
	n.visit_postfix([&](const node & n) { visited_types_after.push_back(n.get_type()); });

	EXPECT_EQ(expected_types_before, visited_types_before);
	EXPECT_EQ(expected_types_after, visited_types_after);
}

TEST_F(test_range_node, transform_and_only_leafes)
{
	// clang-format off
	auto n = node::create_and(
		std::make_unique<node>(node::create_gt(semver("1.2.3"))),
		std::make_unique<node>(node::create_lt(semver("2.3.4")))
		);

	const std::vector<node::type> expected_types_before = {
		node::type::op_gt, node::type::op_lt, node::type::op_and
		};
	const std::vector<node::type> expected_types_after = {
		node::type::op_gt, node::type::op_lt, node::type::op_and
		};
	// clang-format on

	std::vector<node::type> visited_types_before;
	std::vector<node::type> visited_types_after;

	n.visit_postfix([&](const node & n) { visited_types_before.push_back(n.get_type()); });
	n.optimize();
	n.visit_postfix([&](const node & n) { visited_types_after.push_back(n.get_type()); });

	EXPECT_EQ(expected_types_before, visited_types_before);
	EXPECT_EQ(expected_types_after, visited_types_after);
}

TEST_F(test_range_node, transform_and_complex_subtree)
{
	// clang-format off
	auto n = node::create_and(
		std::make_unique<node>(node::create_and(
			std::make_unique<node>(node::create_gt(semver("1.2.3"))),
			std::make_unique<node>(node::create_lt(semver("2.3.4")))
			)),
		std::make_unique<node>(node::create_and(
			std::make_unique<node>(node::create_ge(semver("3.4.5"))),
			std::make_unique<node>(node::create_le(semver("4.5.6")))
			))
		);

	const std::vector<node::type> expected_types_before = {
		node::type::op_gt, node::type::op_lt, node::type::op_and,
		node::type::op_ge, node::type::op_le, node::type::op_and,
		node::type::op_and
		};
	const std::vector<node::type> expected_types_after = {
		node::type::op_gt, node::type::op_lt, node::type::op_and,
		node::type::op_ge, node::type::op_le, node::type::op_and,
		node::type::op_and
		};
	// clang-format on

	std::vector<node::type> visited_types_before;
	std::vector<node::type> visited_types_after;

	n.visit_postfix([&](const node & n) { visited_types_before.push_back(n.get_type()); });
	n.optimize();
	n.visit_postfix([&](const node & n) { visited_types_after.push_back(n.get_type()); });

	EXPECT_EQ(expected_types_before, visited_types_before);
	EXPECT_EQ(expected_types_after, visited_types_after);
}

TEST_F(test_range_node, transform_and_leaf_move_left)
{
	// clang-format off
	auto n = node::create_and(
		std::make_unique<node>(node::create_and(
			std::make_unique<node>(node::create_gt(semver("1.2.3"))),
			std::make_unique<node>(node::create_lt(semver("2.3.4")))
			)),
		std::make_unique<node>(node::create_eq(semver("3.4.5")))
		);

	const std::vector<node::type> expected_types_before = {
		node::type::op_gt, node::type::op_lt, node::type::op_and,
		node::type::op_eq,
		node::type::op_and
		};
	const std::vector<node::type> expected_types_after = {
		node::type::op_eq,
		node::type::op_gt, node::type::op_lt, node::type::op_and,
		node::type::op_and
		};
	// clang-format on

	std::vector<node::type> visited_types_before;
	std::vector<node::type> visited_types_after;

	n.visit_postfix([&](const node & n) { visited_types_before.push_back(n.get_type()); });
	n.optimize();
	n.visit_postfix([&](const node & n) { visited_types_after.push_back(n.get_type()); });

	EXPECT_EQ(expected_types_before, visited_types_before);
	EXPECT_EQ(expected_types_after, visited_types_after);
}

TEST_F(test_range_node, transform_and_smaller_leaf_move_left)
{
	// clang-format off
	auto n = node::create_and(
			std::make_unique<node>(node::create_lt(semver("2.3.4"))),
			std::make_unique<node>(node::create_gt(semver("1.2.3")))
		);

	const std::vector<node::type> expected_types_before = {
		node::type::op_lt, node::type::op_gt, node::type::op_and
		};
	const std::vector<semver> expected_versions_before = {
		semver("2.3.4"), semver("1.2.3")
		};
	const std::vector<node::type> expected_types_after = {
		node::type::op_gt, node::type::op_lt, node::type::op_and
		};
	const std::vector<semver> expected_versions_after = {
		semver("1.2.3"), semver("2.3.4")
		};
	// clang-format on

	std::vector<node::type> visited_types_before;
	std::vector<node::type> visited_types_after;
	std::vector<semver> visited_versions_before;
	std::vector<semver> visited_versions_after;

	n.visit_postfix([&](const node & n) {
		visited_types_before.push_back(n.get_type());
		if (n.is_leaf())
			visited_versions_before.push_back(*n.get_version());
	});
	n.optimize();
	n.visit_postfix([&](const node & n) {
		visited_types_after.push_back(n.get_type());
		if (n.is_leaf())
			visited_versions_after.push_back(*n.get_version());
	});

	EXPECT_EQ(expected_types_before, visited_types_before);
	EXPECT_EQ(expected_types_after, visited_types_after);
	EXPECT_EQ(expected_versions_before, visited_versions_before);
	EXPECT_EQ(expected_versions_after, visited_versions_after);
}

TEST_F(test_range_node, transform_or_smaller_leaf_move_left)
{
	// clang-format off
	auto n = node::create_or(
			std::make_unique<node>(node::create_lt(semver("2.3.4"))),
			std::make_unique<node>(node::create_gt(semver("1.2.3")))
		);

	const std::vector<node::type> expected_types_before = {
		node::type::op_lt, node::type::op_gt, node::type::op_or
		};
	const std::vector<semver> expected_versions_before = {
		semver("2.3.4"), semver("1.2.3")
		};
	const std::vector<node::type> expected_types_after = {
		node::type::op_gt, node::type::op_lt, node::type::op_or
		};
	const std::vector<semver> expected_versions_after = {
		semver("1.2.3"), semver("2.3.4")
		};
	// clang-format on

	std::vector<node::type> visited_types_before;
	std::vector<node::type> visited_types_after;
	std::vector<semver> visited_versions_before;
	std::vector<semver> visited_versions_after;

	n.visit_postfix([&](const node & n) {
		visited_types_before.push_back(n.get_type());
		if (n.is_leaf())
			visited_versions_before.push_back(*n.get_version());
	});
	n.optimize();
	n.visit_postfix([&](const node & n) {
		visited_types_after.push_back(n.get_type());
		if (n.is_leaf())
			visited_versions_after.push_back(*n.get_version());
	});

	EXPECT_EQ(expected_types_before, visited_types_before);
	EXPECT_EQ(expected_types_after, visited_types_after);
	EXPECT_EQ(expected_versions_before, visited_versions_before);
	EXPECT_EQ(expected_versions_after, visited_versions_after);
}

TEST_F(test_range_node, transform_or_sort_leafs_across_or_nodes)
{
	// clang-format off
	auto n = node::create_or(
			std::make_unique<node>(node::create_or(
				std::make_unique<node>(node::create_lt(semver("4.5.6"))),
				std::make_unique<node>(node::create_gt(semver("3.4.5")))
				)),
			std::make_unique<node>(node::create_or(
				std::make_unique<node>(node::create_le(semver("2.3.4"))),
				std::make_unique<node>(node::create_ge(semver("1.2.3")))
				))
		);

	const std::vector<node::type> expected_types_before = {
		node::type::op_lt, node::type::op_gt, node::type::op_or,
		node::type::op_le, node::type::op_ge, node::type::op_or,
		node::type::op_or
		};
	const std::vector<semver> expected_versions_before = {
		semver("4.5.6"), semver("3.4.5"), semver("2.3.4"), semver("1.2.3")
		};
	const std::vector<node::type> expected_types_after = {
		node::type::op_ge, node::type::op_le, node::type::op_or,
		node::type::op_gt, node::type::op_lt, node::type::op_or,
		node::type::op_or
		};
	const std::vector<semver> expected_versions_after = {
		semver("1.2.3"), semver("2.3.4"), semver("3.4.5"), semver("4.5.6")
		};
	// clang-format on

	std::vector<node::type> visited_types_before;
	std::vector<node::type> visited_types_after;
	std::vector<semver> visited_versions_before;
	std::vector<semver> visited_versions_after;

	n.visit_postfix([&](const node & n) {
		visited_types_before.push_back(n.get_type());
		if (n.is_leaf())
			visited_versions_before.push_back(*n.get_version());
	});
	n.optimize();
	n.visit_postfix([&](const node & n) {
		visited_types_after.push_back(n.get_type());
		if (n.is_leaf())
			visited_versions_after.push_back(*n.get_version());
	});

	EXPECT_EQ(expected_types_before, visited_types_before);
	EXPECT_EQ(expected_types_after, visited_types_after);
	EXPECT_EQ(expected_versions_before, visited_versions_before);
	EXPECT_EQ(expected_versions_after, visited_versions_after);
}
*/
}

