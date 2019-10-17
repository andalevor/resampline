#ifndef PARAMS_HPP
#define PARAMS_HPP

#include <string>

struct params{
	params() : start(0), end(0) {}
	std::string in_file, out_file;
	double step, start, end;
};

params parse_params(int argc, char **argv);

#endif //PARAMS_HPP
