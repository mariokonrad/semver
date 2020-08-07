#include <semver/semver.hpp>

int main(int argc, char ** argv)
{
	if (argc != 3)
		return 2;

	const auto v1 = semver::semver(argv[1], true);
	const auto v2 = semver::semver(argv[2], true);

	if (!v1.ok() || !v2.ok())
		return 3;

	return compare(v1, v2);
}

