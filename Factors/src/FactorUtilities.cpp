#include "FactorUtilities.h"
#include "Factor.h"
#include "PGMUtilities.h"
#include "StatUtilities.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <string>
#include <vector>


using namespace std;
//Note: i is supposed to be starting at 1, NOT 0
int IndexToAssignment(int i, vector<int> factorcard, vector<int> *result)
{
	vector <int> cardCumProd;
	vector <int>::iterator iterCumProd;
	int cardLength = factorcard.capacity();
	int counter;
	int rElement;
	int valSize = accumulate(factorcard.begin(), factorcard.end(), 1, multiplies<int>());

	if (i > valSize)
	{
		cerr << "input index number is greater than the prod of card." << endl;
		return -1;
	}

	cardCumProd = factorcard;
	cardCumProd.insert(cardCumProd.begin(), 1);
	cardCumProd.pop_back();
	iterCumProd = partial_sum(cardCumProd.begin(), cardCumProd.end(), cardCumProd.begin(), multiplies <int>());
	result->clear();

	for (counter = 0; counter < cardLength; counter++)
	{
		rElement = (((i - 1) / cardCumProd[counter]) % factorcard[counter]) + 1;
		result->push_back(rElement);
	}
	return 0;

}

int AssignmentToIndex(std::vector<int> assignment, std::vector<int> factorcard)
{
	int idx = 0;
	vector <int> cardCumProd;
	int cardLength = factorcard.capacity();
	int counter;

	//Validating the input size. Assignment and Factor card size MUST matched!
	if (assignment.capacity() != factorcard.capacity())
	{
		cout << "Assignment and Card size NOT Match!!!" << endl;
		return -1; //Size NOT Matched!!!
	}

	cardCumProd = factorcard;
	cardCumProd.insert(cardCumProd.begin(), 1);
	cardCumProd.pop_back();
	partial_sum(cardCumProd.begin(), cardCumProd.end(), cardCumProd.begin(), multiplies <int>());

	for (counter = 0; counter < cardLength; counter++)
		idx = idx + cardCumProd[counter] * (assignment[counter] - 1);
	idx += 1;
	return idx;
}


Factor operator*(Factor A, Factor B)
{  
	vector <int> varAList=A.GetVar();
	vector <int> cardAList=A.GetCard();
	vector <int> varBList=B.GetVar();
	vector <int> cardBList=B.GetCard();
	vector <int>::iterator it;
	vector <int> rowRec;
	vector <int> assignmentA(varAList.size());
	vector <int> assignmentB(varBList.size());
	int idx = 0;
	vector <int> varList=varAList;
	vector <int> cardList=cardAList;
	vector <int> BPos(varBList.size());
	vector <int> APos(varAList.size());
	double valA = 1;
	double valB = 1;
	Factor newF;
	int valSize = 0;
	
	if (A.isempy())
	{
		//cout << "Warining: First Factor is empty, return second Factor" << endl;
		return B;
	}
	
	if (B.isempy())
	{
		//cout << "Warining: Second Factor is empty, return first Factor" << endl;
		return A;
	}


	//A's var is the same as the first n vars (n=A.var.size()) of the production
	generate(APos.begin(), APos.end(), [&idx] {return idx++; });

	//Construct new factor var and card from A and B
	for (int x=0;x<(int)varBList.size();x++)
	{
		it = find(varAList.begin(), varAList.end(), varBList[x]);
		if (it == varAList.end())
		{
			varList.push_back(varBList[x]);
			cardList.push_back(cardBList[x]);
			//pos = varList.size() - 1;
			BPos[x]=varList.size()-1;
		}
		else
		{
			//Check out whether the same variables' card is the same
			if (cardBList[x]!=cardAList[it-varAList.begin()])
			{
				cout << "Error: Variable '" << varBList[x] << "' in both A and B has different card value!" << endl;
				return newF;
			}
			BPos[x]= it - varAList.begin();
		}
	}

	valSize = accumulate(cardList.begin(), cardList.end(), 1, multiplies<int>());
	newF.FactorInit(varList, cardList, 1);

	for (int counter = 0; counter < valSize; counter++)
	{
		IndexToAssignment(counter + 1, cardList, &rowRec);
		assignmentA = SubVector(rowRec, APos);
		idx = AssignmentToIndex(assignmentA, cardAList);
		valA = A.val[idx-1];
		
		assignmentB = SubVector(rowRec, BPos);
		idx = AssignmentToIndex(assignmentB, cardBList);
		valB = B.val[idx-1];
		newF.val[counter] = valA*valB;
	}
	return newF;
}

bool eq(Factor A, Factor B, double precision)
{
	vector <int> vo = A.GetVar();
	vector <int> tmpVar = B.GetVar();
	double dev = 0.0;
	double n = A.val.size();
	if (A.val.size() != B.val.size())
	{
		cout << "Factor Val size NOT matched!" << endl;
		return false;
	}

	if (vo != tmpVar)
	{
		sort(tmpVar.begin(), tmpVar.end());
		sort(vo.begin(), vo.end());
		if (vo != tmpVar)
		{
			cout << "Factor vars NOT matched!" << endl;
			return false;
		}
		A.ChangeVarOrder(vo);
		B.ChangeVarOrder(vo);
	}
	if (A.GetCard() != B.GetCard())
	{
		cout << "Factor Card NOT matched!" << endl;
		return false;
	}
	for (int t = 0; t < (int)n; t++)
		if (A.val[t]!=0.0||B.val[t]!=0.0)
			dev+=pow(2.0*(A.val[t] - B.val[t])/(A.val[t]+B.val[t]), 2.0);
	dev = sqrt(dev / n);
	if (dev < precision)
		return true;
	else
	{
		cout << "Deviation between A and B val are over " << precision << endl;
		return false;
	}
}

bool operator==(Factor A, Factor B)
{
	double precision = 1e-10;
	return eq(A,B,precision);
}

bool operator!=(Factor A, Factor B)
{
	return !(A==B);
}

Factor ComputeJointDistribution(vector <Factor> f)
{
	int fNum = f.size();
	Factor fJointD;
	for (int fCount = 0; fCount < fNum; fCount++)
		fJointD = fJointD*f[fCount];
	return fJointD;
}

Factor ComputeMarginal(std::vector<int> varList, std::vector<Factor> factors, std::map<int, int> evidence)
{
	Factor jointD = ComputeJointDistribution(factors);
	Factor jointDE = jointD.ObserveEvidence(evidence);
	vector <int> jointDVar = jointD.GetVar();
	vector <int>::iterator itVar;
	vector <int> vMargins;
	for (int counter = 0; counter < (int)jointDVar.size(); counter++)
	{
		itVar = find(varList.begin(), varList.end(), jointDVar[counter]);
		if (itVar == varList.end())
			vMargins.push_back(jointDVar[counter]);
	}
	int totalVarNum = vMargins.size() + varList.size();
	if (totalVarNum > (int)jointDVar.size())
	{
		cout << "Error: There is at least 1 variable is NOT in the Joint Distribution! Joint Distribution returned." << endl;
		return jointD;
	}
	Factor M = jointDE.FactorMarginalization(vMargins);
	M.val = Normalize(M.val);

	return M;
}

