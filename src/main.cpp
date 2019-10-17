#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>
#include <cassert>
#include "params.hpp"
#include "error.hpp"

using namespace std;

vector<pair<double,double>> get_data(params &p);
vector<pair<double,double>> parse_input(istream &input);
vector<pair<double,double>> resample(vector<pair<double,double>> &pairs,
									 params &p);
void data_output(vector<pair<double,double>> &result, params &p);
void print_result(vector<pair<double,double>> &result, ostream &out);

int main(int argc, char **argv)
{
	try {
		if (argc < 2)
			throw error(string("Not enouth parameters.") +
					   	"\n\nUsage: " + argv[0] + " if=\"inputfile\" "
	  					"of=\"output_file\" step=\"number\" [OPTIONAL]"
						"\n\nIf either or both input and output files are not set,"
						"\nSTDIN or STDOUT respectively will be used."
						"\nEach input line should consist of X and Y pairs "
						"with witespace delimiter."
						"\n\nOptional parameters:"
						"\n\tstart=\tValue in units(meters or feet). "
						"Could be used to set start point"
						"\n\t\tif first point in input file is not the start. "
						"\n\t\tIt is assumed that first point in input is 0."
						"\n\t\tSo everything before first point should be set "
						"as negative value."
						"\n\tend=\tValue in units(meters or feet). "
						"Could be used to set end point"
						"\n\t\tif last point in input is not the end."
						"\n\t\tIt is recommended to always set the \"end=\" value.");

		params par = parse_params(argc, argv);
		auto pairs = get_data(par);
		if (pairs.size() < 2)
			throw error("Not enouth data");
		auto result = resample(pairs, par);
		data_output(result, par);
	} catch(exception& e) {
		cerr << "Error occurred:\n" << e.what() << endl;
		return 1;
	}
}

vector<pair<double,double>> get_data(params &p)
{
	vector<pair<double,double>> pairs;
	if (p.in_file.empty()) {
		pairs = parse_input(cin);
	} else {
		ifstream in_file(p.in_file);
		if (in_file)
			pairs = parse_input(in_file);
	}
	return pairs;
}

vector<pair<double,double>> parse_input(istream &input)
{
	vector<pair<double,double>> vec;
	input.exceptions(istream::badbit | istream::failbit);
	try {
		double first, second;
		char ch;
		while (true) {
			input >> first >> second;
			vec.push_back(make_pair(first,second));
			input.get(ch);
			if (ch != '\n' && ch != '\r')
				throw error("More then two numbers in row");
		}
	} catch (ifstream::failure &e) {
		if (!input.eof())
			throw ifstream::failure("No such file of wrong data format");
	}
	return vec;
}

vector<pair<double,double>> resample(vector<pair<double,double>> &pairs,
									 params &p)
{
	vector<pair<double,double>> result;
	double first_x, first_y, second_x, second_y, azimuth, length = 0, dx, dy, x, y,
		   third_x, third_y, alpha, beta, gamma, a, b, c, total = 0;
	int steps;
	decltype(pairs.size()) counter = 0;

	//if start is far from first point we need find line with start point
	do {
		p.start -= length;
		first_x = pairs[counter].first;
		first_y = pairs[counter].second;
		++counter;
		second_x = pairs[counter].first;
		second_y = pairs[counter].second;
		length = sqrt(pow(first_x - second_x, 2) + pow(first_y - second_y, 2));
	} while (length - p.start <= 0 && counter + 1 < pairs.size());
	length -= p.start;
	x = first_x;
	y = first_y;
	//find azimuth for first line
	azimuth = atan2(second_y - first_y, second_x - first_x);
	if (p.start) {
		x += cos(azimuth) * p.start;
		y += sin(azimuth) * p.start;
	}
	result.push_back(make_pair(x, y));
	steps = length / p.step;
	dx = cos(azimuth) * p.step;
	dy = sin(azimuth) * p.step;
	for (int i = 0; i < steps; ++i) {
		x += dx;
		y += dy;
		result.push_back(make_pair(x, y));
		total += p.step;
	}
	for (decltype(pairs.size()) i = counter + 1; i < pairs.size(); ++i) {
		third_x = pairs[i].first;
		third_y = pairs[i].second;
		//consider a triangle with sides a, b, c and angles alpha, beta, gamma
		//b is what left on first line, a -- step
		//we need to find c -- part of second line from begining
		//where we need to place next point
		a = p.step;
		b = length - steps * p.step;
		assert(b < a); //b should be never more then a
		//find angle between lines as difference of their angles
		//which we can find from their slopes
		alpha = fabs(atan2(first_y - second_y, first_x - second_x) -
					 atan2(third_y - second_y, third_x - second_x));
		//alpha could be more than 180 degrees
		if (alpha > M_PI)
			alpha = 2 * M_PI - alpha;
		//from sines theorem
		beta = asin(b / a * sin(alpha));
		//sum of triangles angles is 180
		gamma = M_PI - alpha - beta;
		//from sines theorem
		c = a * sin(gamma) / sin(alpha);
		//find dx and dy for first point
		azimuth = atan2(third_y - second_y, third_x - second_x);
		dx = cos(azimuth) * c;
		dy = sin(azimuth) * c;
		x = second_x + dx;
		y = second_y + dy;
		result.push_back(make_pair(x, y));
		total += p.step;
		length = sqrt(pow(third_x - second_x, 2) + pow(third_y - second_y, 2)) - c;
		if (p.end && i == pairs.size() - 1)
			length = p.end - p.start - total;
		steps = length / p.step;
		dx = cos(azimuth) * p.step;
		dy = sin(azimuth) * p.step;
		for (int i = 0; i < steps; ++i) {
			x += dx;
			y += dy;
			result.push_back(make_pair(x, y));
			total += p.step;
		}
		first_x = second_x;
		first_y = second_y;
		second_x = third_x;
		second_y = third_y;
	}
	if (p.end) {
		while (total < p.end - p.start) {
			x += dx;
			y += dy;
			result.push_back(make_pair(x, y));
			total += p.step;
		}
	}

	return result;
}

void data_output(vector<pair<double,double>> &result, params &p)
{
	if (p.out_file.empty()) {
		print_result(result, cout);
	} else {
		ofstream out_file(p.out_file);
		if (out_file)
			print_result(result, out_file);
	}
}

void print_result(vector<pair<double,double>> &result, ostream &out)
{
	vector<int> field_length;
	field_length.push_back(to_string(result.size()).length());
	field_length.push_back(to_string(static_cast<long>(result[0].first)).length() + 2);
	field_length.push_back(to_string(static_cast<long>(result[0].second)).length() + 2);

	out.exceptions(istream::badbit | istream::failbit);
	decltype(result.size()) counter = 0;
	for (const auto &v : result)
		out << setprecision(1) << fixed << " "
		   	<< setw(field_length[0]) << ++counter << " "
			<< setw(field_length[1]) << v.first   << " "
			<< setw(field_length[2]) << v.second  << endl;
}
