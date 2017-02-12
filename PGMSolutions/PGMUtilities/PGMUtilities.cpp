#include "PGMUtilities.h"
#include <vector>
#include <numeric>
#include <algorithm>
using namespace std;
std::vector<double> Normalize(std::vector<double> input)
{
	//Normalize a double vector
	vector <double> returnVal(input.size());
	int n = 0;
	double z = accumulate(input.begin(), input.end(), 0.0);
	for_each(input.begin(), input.end(), [&n, &returnVal, z](double a) {returnVal[n] = a / z; n++; });
	return returnVal;
}

double StdDeviation(std::vector<double> input)
{
	double n = (double)input.size();
	double ave = accumulate(input.begin(), input.end(), 0.0) / n;
	double dev = 0.0;
	vector <double> output;
	for (auto x : input)
		dev += pow((x - ave), 2.0);
	return sqrt(dev / n);
}
