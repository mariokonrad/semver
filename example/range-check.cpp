#include <semver/range.hpp>

int main(int argc, char ** argv)
{
	if (argc != 3)
		return 2;

	const auto r = semver::range(argv[1]);
	if (!r.ok())
		return 3;

	const auto v = semver::semver(argv[2]);
	if (!v.ok())
		return 4;

	return r.satisfies(v) ? 0 : -1;
}

