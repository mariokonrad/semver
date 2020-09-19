#include <semver/detail/range_lexer.hpp>
#include <gtest/gtest.h>

#define EXPECT_SV(s, e) EXPECT_EQ(std::string_view(s), (e))

namespace
{
using lexer = semver::detail::range_lexer;

class test_range_lexer : public ::testing::Test {};

TEST_F(test_range_lexer, empty_string)
{
	lexer l("");

	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, spaces_only)
{
	lexer l("");

	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, single_dash)
{
	lexer l("-");

	EXPECT_EQ(lexer::token::dash, l.scan());
	EXPECT_SV("", l.text().op);
	EXPECT_SV("", l.text().major);
	EXPECT_SV("", l.text().minor);
	EXPECT_SV("", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, multiple_dashes)
{
	lexer l("- -  -");

	EXPECT_EQ(lexer::token::dash, l.scan());
	EXPECT_EQ(lexer::token::dash, l.scan());
	EXPECT_EQ(lexer::token::dash, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, logical_or)
{
	lexer l("||");

	EXPECT_EQ(lexer::token::logical_or, l.scan());
	EXPECT_SV("", l.text().op);
	EXPECT_SV("", l.text().major);
	EXPECT_SV("", l.text().minor);
	EXPECT_SV("", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, logical_or_failure)
{
	lexer l("|");

	EXPECT_EQ(lexer::token::error, l.scan());
}

TEST_F(test_range_lexer, repeated_eof)
{
	lexer l("");

	EXPECT_EQ(lexer::token::eof, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, repeated_error)
{
	lexer l("|");

	EXPECT_EQ(lexer::token::error, l.scan());
	EXPECT_EQ(lexer::token::error, l.scan());
	EXPECT_EQ(lexer::token::error, l.scan());
}

TEST_F(test_range_lexer, partial)
{
	lexer l("1.2.3");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_SV("", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, caret_partial)
{
	lexer l("^1.2.3");

	EXPECT_EQ(lexer::token::caret_partial, l.scan());
	EXPECT_SV("^", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, tilde_partial)
{
	lexer l("~1.2.3");

	EXPECT_EQ(lexer::token::tilde_partial, l.scan());
	EXPECT_SV("~", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_lt)
{
	lexer l("<1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_SV("<", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_le)
{
	lexer l("<=1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_SV("<=", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_gt)
{
	lexer l(">1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_SV(">", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_ge)
{
	lexer l(">=1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_SV(">=", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_eq)
{
	lexer l("=1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_SV("=", l.text().op);
	EXPECT_SV("1", l.text().major);
	EXPECT_SV("2", l.text().minor);
	EXPECT_SV("3", l.text().patch);
	EXPECT_SV("", l.text().prerelease);
	EXPECT_SV("", l.text().build);
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_invalid)
{
	EXPECT_EQ(lexer::token::error, lexer("= 1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("< 1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("> 1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("<= 1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer(">= 1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("!=1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("==1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("=>1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("=<1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("!1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("&1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("%1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("$1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("#1.2.3").scan());
	EXPECT_EQ(lexer::token::error, lexer("@1.2.3").scan());
}

TEST_F(test_range_lexer, full_version)
{
	lexer l("1.2.3");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_TRUE(l.text().full_version);
}

TEST_F(test_range_lexer, full_version_but_wildcard_on_major)
{
	lexer l("*.2.3");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_FALSE(l.text().full_version);
}

TEST_F(test_range_lexer, full_version_but_wildcard_on_minor)
{
	lexer l("1.*.3");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_FALSE(l.text().full_version);
}

TEST_F(test_range_lexer, full_version_but_wildcard_on_patch)
{
	lexer l("1.2.*");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_FALSE(l.text().full_version);
}

TEST_F(test_range_lexer, full_version_with_prerelease)
{
	lexer l("1.2.3-alpha");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_TRUE(l.text().full_version);
}

TEST_F(test_range_lexer, not_full_version_major_only)
{
	lexer l("1");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_FALSE(l.text().full_version);
}

TEST_F(test_range_lexer, not_version_major_minor_only)
{
	lexer l("1.2");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_FALSE(l.text().full_version);
}

TEST_F(test_range_lexer, not_version_major_as_wildcard)
{
	lexer l("*");

	EXPECT_EQ(lexer::token::partial, l.scan());
	EXPECT_FALSE(l.text().full_version);
}
}

