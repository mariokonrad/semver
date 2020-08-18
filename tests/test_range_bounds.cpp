#include <semver/range.hpp>
#include <gtest/gtest.h>
#include <array>

namespace
{
using semver::semver;

class test_range_bounds : public ::testing::Test
{
};

TEST_F(test_range_bounds, xrange_empty_string)
{
	const auto s = "";

	EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
	EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
}

TEST_F(test_range_bounds, xrange_wildcard_position)
{
	// clang-format off
	static const std::array<std::string, 8> tab = {
		"1.2.3",
		"1.2._",
		"1._.3",
		"1._._",
		"_.2.3",
		"_.2._",
		"_._.3",
		"_._._",
	};
	// clang-format on

	for (const char * p = "*xX\0"; p && *p; ++p) {
		std::array<std::string, 8> t = tab;
		for (auto & s : t)
			std::transform(begin(s), end(s), begin(s),
				[w = *p](std::string::value_type c) { return (c == '_') ? w : c; });

		EXPECT_TRUE(::semver::detail::lower_bound(t[0]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[1]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[2]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[3]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[4]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[5]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[6]).ok());
		EXPECT_TRUE(::semver::detail::lower_bound(t[7]).ok());
	}
}

TEST_F(test_range_bounds, xrange_major_only)
{
	const auto s = "1";

	EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
	EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(s));
}

TEST_F(test_range_bounds, xrange_major_minor_only)
{
	const auto s = "1.2";

	EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(s));
	EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(s));
}

TEST_F(test_range_bounds, xrange_wildcard_on_major)
{
	const auto s = "*";

	EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
	EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor)
{
	{
		const auto s = "1.*";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.x";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.X";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(s));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_only)
{
	{
		const auto s = "*.2.3";

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "x.2.3";

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "X.2.3";

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_only)
{
	{
		const auto s = "1.*.3";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.x.3";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.X.3";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("2.0.0-0"), ::semver::detail::upper_bound(s));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_patch_only)
{
	{
		const auto s = "1.2.*";

		EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.2.x";

		EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.2.X";

		EXPECT_EQ(semver("1.2.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(s));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_minor_and_patch)
{
	{
		const auto s = "1.*.*";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.x.x";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "1.X.X";

		EXPECT_EQ(semver("1.0.0"), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver("1.3.0-0"), ::semver::detail::upper_bound(s));
	}
}

TEST_F(test_range_bounds, xrange_wildcard_on_major_and_minor_and_patch)
{
	{
		const auto s = "*.*.*";

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "x.x.x";

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
	}
	{
		const auto s = "X.X.X";

		EXPECT_EQ(semver::min(), ::semver::detail::lower_bound(s));
		EXPECT_EQ(semver::max(), ::semver::detail::upper_bound(s));
	}
}

// TODO: tilde ranges

// TODO: caret ranges
}

