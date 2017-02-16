/*
This sample is the same as the FactorTutorial.m from the week 1 assignment of 
Probabilistic Graphical Models 1:Representation
The course is taught by Prof. Daphne Koller.
Detail of the factor description, please refer to the readme.md in the same directory of this file. 
*/
#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <algorithm>
#include <map>
#include "Factor.h"
#include "FactorUtilities.h"
using namespace std;

int main()
{
	char input;
	string lineSep(30, '-');
	map <int, int> evidence;
	vector <Factor> f(3);
	f[0].FactorInit({ 1 }, { 2 }, { 0.11, 0.89 });
	f[0].SetFactorName("Factors.INPUT(1)");
	f[1].FactorInit({ 2,1 }, { 2,2 }, { 0.59,0.41, 0.22, 0.78 });
	f[1].SetFactorName("Factors.INPUT(2)");
	f[2].FactorInit({ 3,2 }, { 2,2 }, { 0.39, 0.61, 0.06, 0.94 });
	f[2].SetFactorName("Factors.INPUT(3)");

	Factor result;

	//Factor Product
	//FACTORS.PRODUCT = FactorProduct(FACTORS.INPUT(1), FACTORS.INPUT(2));
	//The factor defined here is correct to 4 decimal places.
	//FACTORS.PRODUCT = struct('var', [1, 2], 'card', [2, 2], 'val', [0.0649, 0.1958, 0.0451, 0.6942]);
	//We are using product=f1*f2 in this C++ implementation

	cout << lineSep << "\nTest for Factor Product between f[0]xf[1] \nShould be [0.0649, 0.1958, 0.0451, 0.6942]" << endl;
	result = f[0] * f[1];
	result.SetFactorName("Product of Factor(1)xFactor(2)");
	result.Show();

	//Factor Marginalization
	//FACTORS.MARGINALIZATION = FactorMarginalization(FACTORS.INPUT(2), [2]);
	//FACTORS.MARGINALIZATION = struct('var', [1], 'card', [2], 'val', [1 1]);
	cout << lineSep << "\nTest for Factor Marginalization for f[1] of variable 2 \nShould be var:1 card 2 val:[1 1]" << endl;
	result = f[1].FactorMarginalization({ 2 });
	result.SetFactorName("Factor Marginalization for Factor[2]");
	result.Show();

	//Observe Evidence
	//FACTORS.EVIDENCE = ObserveEvidence(FACTORS.INPUT, [2 1; 3 2]);
	//FACTORS.EVIDENCE(1) = struct('var', [1], 'card', [2], 'val', [0.11, 0.89]);
	//FACTORS.EVIDENCE(2) = struct('var', [2, 1], 'card', [2, 2], 'val', [0.59, 0, 0.22, 0]);
	//FACTORS.EVIDENCE(3) = struct('var', [3, 2], 'card', [2, 2], 'val', [0, 0.61, 0, 0]);
	cout << lineSep << "\nTest for ObserveEvidence for f set with evidence: [var2=1;var3=2]" <<endl;
	evidence.insert(make_pair(2, 1));
	evidence.insert(make_pair(3, 2));

	for (int counter = 0; counter < 3; counter++)
	{
		result = f[counter].ObserveEvidence(evidence);
		result.SetFactorName(f[counter].GetFactorName() + " with Evidence");
		result.Show();
	}




	//Compute Joint Distribution
	//FACTORS.JOINT = ComputeJointDistribution(FACTORS.INPUT);
	//FACTORS.JOINT = struct('var', [1, 2, 3], 'card', [2, 2, 2], 'val', [0.025311, 0.076362, 0.002706, 0.041652, 0.039589, 0.119438, 0.042394, 0.652548]);
	cout << lineSep << "\nTest for Compute Joint Distribution for f set\nVal Should be [0.025311, 0.076362, 0.002706, 0.041652, 0.039589, 0.119438, 0.042394, 0.652548]" << endl;
	result = ComputeJointDistribution(f);
	result.SetFactorName("Joint Distribution");
	result.Show();

	//Compute Marginal
	//FACTORS.MARGINAL = ComputeMarginal([2, 3], FACTORS.INPUT, [1, 2]);
	//FACTORS.MARGINAL = struct('var', [2, 3], 'card', [2, 2], 'val', [0.0858, 0.0468, 0.1342, 0.7332]);
	cout << lineSep << "\nTest for Compute Marginal for f set with evidence [1,2]\nVal Should be [0.0858, 0.0468, 0.1342, 0.7332]" << endl;
	evidence.clear();
	evidence.insert(make_pair(1, 2));
	result = ComputeMarginal({ 2,3 }, f, evidence);
	result.SetFactorName("Compute Marginal");
	result.Show();

	cin >> input;
	return 0;
}