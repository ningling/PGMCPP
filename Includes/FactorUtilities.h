#pragma once
#include <vector>
#include "Factor.h"

int IndexToAssignment(int i, std::vector <int> factorcard, std::vector <int> *result);
int AssignmentToIndex(std::vector<int> assignment, std::vector<int> factorcard);
Factor operator*(Factor A, Factor B);
bool eq(Factor A, Factor B, double precision);
bool operator==(Factor A, Factor B);
bool operator!=(Factor A, Factor B);
Factor ComputeJointDistribution(std::vector <Factor> f);
Factor ComputeMarginal(std::vector <int> varList, std::vector <Factor> factors, std::map<int, int> evidence);
