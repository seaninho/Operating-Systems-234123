#ifndef __UTILS_H
#define __UTILS_H
#include "Headers.hpp"

namespace utils {
	// Read all non-empty lines from file as strings with no carriage returns, and returns them in a std::vector
	vector<string> read_lines(const string& filename);
	// Returns a string array that contains the substrings in the string s that are delimited by the char delimiter
	vector<string> split(const string& s, char delimiter); //Splits a string

	//Returns total lines in filename
	uint num_lines(const string& filename);
	//Returns total cols in filename
	uint num_cols(const string& filename);
}

string repeat(string str, const size_t n);
string operator*(string str, size_t n);



#endif
