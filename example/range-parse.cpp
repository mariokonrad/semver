#include <semver/range.hpp>

int main(int argc, char ** argv)
{
	if (argc != 2)
		return 2;

	const auto r = semver::range(argv[1]);

	if (!r.ok())
		return 3;

	return 0;
}

