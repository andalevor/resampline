#include <vector>
#include "params.hpp"
#include "error.hpp"

using std::vector;
using std::string;

params parse_params(int argc, char **argv)
{
	params result;
	vector<string> par = {"if=","of=","step=","start=","end="};
	vector<bool> found(par.size(), false);
	for (int i = 1; i < argc; ++i) {
		if (!found[0] && !par[0].compare(0, par[0].length(),
 										 argv[i], 0, par[0].length())) {
			found[0] = true;
			result.in_file = argv[i] + par[0].length();
		} else if (!found[1] && !par[1].compare(0, par[1].length(),
 												argv[i], 0, par[1].length())) {
			found[1] = true;
			result.out_file = argv[i] + par[1].length();
		} else if (!found[2] && !par[2].compare(0, par[2].length(),
 												argv[i], 0, par[2].length())) {
			found[2] = true;
			result.step = std::stod(argv[i] + par[2].length());
		} else if (!found[3] && !par[3].compare(0, par[3].length(),
 												argv[i], 0, par[3].length())) {
			found[3] = true;
			result.start = std::stod(argv[i] + par[3].length());
		} else if (!found[4] && !par[4].compare(0, par[4].length(),
 												argv[i], 0, par[4].length())) {
			found[4] = true;
			result.end = std::stod(argv[i] + par[4].length());
		}
	}

	if (!found[2])
		throw error(par[2] + " parameter not found");

	if (result.end && result.end < result.start)
		throw error("End point less then start");

	return result;
}
