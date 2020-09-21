#include <semver/semver.hpp>
#include <semver/range.hpp>

int main(int, char **)
{
	const auto v = semver::semver("1.2.3");
	const auto r = semver::range("1.0.0 - 2.0.0");

	return r.satisfies(v) ? 0 : -1;
}

