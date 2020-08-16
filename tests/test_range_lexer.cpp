#include <semver/range.hpp>
#include <gtest/gtest.h>

namespace
{
using lexer = semver::detail::lexer;

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
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, caret_partial)
{
	lexer l("^1.2.3");

	EXPECT_EQ(lexer::token::caret_partial, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, tilde_partial)
{
	lexer l("~1.2.3");

	EXPECT_EQ(lexer::token::tilde_partial, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_lt)
{
	lexer l("<1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_le)
{
	lexer l("<=1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_gt)
{
	lexer l(">1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_ge)
{
	lexer l(">=1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
	EXPECT_EQ(lexer::token::eof, l.scan());
}

TEST_F(test_range_lexer, op_partial_eq)
{
	lexer l("=1.2.3");

	EXPECT_EQ(lexer::token::op_partial, l.scan());
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
}

