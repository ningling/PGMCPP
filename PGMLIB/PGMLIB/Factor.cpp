/********************************************************************************
ToDo Lists:
1. Error Handling
1.1 input Assignment is out of range of card.
1.2 var,vo elements should be unique

2. The for loop in the following code should be optimized for parallel computation. 
*********************************************************************************/

#include "Factor.h"
#include "FactorUtilities.h"
#include "PGMUtilities.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <string>
#include <utility>
#include <algorithm>
using namespace std;

Factor::Factor()
{
}

Factor::Factor(std::vector<int> varList, std::vector<int> cardList, vector<double> value)
{
	FactorInit(varList, cardList, 1.0);
	if (value.size() != val.size())
		cout << "Error: val size NOT matched! val is default value!" << endl;
	else
		val = value;
}

/////////////////////////////////////////////////////////////////
/*
return values
0: normal
-1: vo or size is NOT matched with varLength
-2: var element is NOT found in vo
*/
int Factor::GetValueOfAssignment(vector <int> A, vector <int> vo, double *value)
{	
	int idx = 0;
	int pos = 0;
	vector <int> newAssignment = A;
	vector <int>::iterator it;
	
	if ((A.size() != varLength)||vo.size()!=varLength)
	{
		cout << "Error, input Var size is NOT matched with this factor var List" << endl;
		return -1;
	}
	if (vo != var)
	{
		newAssignment.clear();
		for (idx = 0; idx < varLength; idx++)
		{
			it = find(vo.begin(), vo.end(), var[idx]);
			
			if (it != vo.end())
			{
				pos = it - vo.begin();
				newAssignment.push_back(A[pos]);
			}
			else 
			{
				cout << "Error, element " << var[idx] << " NOT found in vo" << endl;
				return -2;
			}
		}
	}

	idx = AssignmentToIndex(newAssignment, card);
	*value = val[idx-1];
	return 0;
}

int Factor::GetValueOfAssignment(std::vector<int> A, double *value)
{
	return GetValueOfAssignment(A, var, value);
}

int Factor::SetValueOfAssignment(double value, std::vector<int> A)
{
	return SetValueOfAssignment(value, A, var);
}

int Factor::SetValueOfAssignment(double value, vector <int> A, vector <int> vo)
{
	int idx = 0;
	int pos = 0;
	vector <int> newAssignment = A;
	vector <int>::iterator it;

	if ((A.size() != varLength) || vo.size() != varLength)
	{
		cout << "Error, input Var size is NOT matched with this factor var List" << endl;
		return -1;
	}
	if (vo != var)
	{
		newAssignment.clear();
		for (idx = 0; idx < varLength; idx++)
		{
			it = find(vo.begin(), vo.end(), var[idx]);

			if (it != vo.end())
			{
				pos = it - vo.begin();
				newAssignment.push_back(A[pos]);
			}
			else
			{
				cout << "Error, element " << var[idx] << " NOT found in vo" << endl;
				return -2;
			}
		}
	}

	idx = AssignmentToIndex(newAssignment, card);
	val[idx-1] = value;
	return 0;
}

int Factor::ChangeVarOrder(std::vector<int> vo)
{
	vector <double> newVal = val;
	vector <int> newCard;
	vector <int> posMap;
	int varNum = var.size();
	int valSize = val.size();
	int valIdx;
	vector <int> tmpVO = vo;
	vector <int> tmpVar = var;
	vector <int> assignment;
	vector <int>::iterator intIter;
	sort(tmpVar.begin(), tmpVar.end());
	sort(tmpVO.begin(), tmpVO.end());
	if (tmpVO != tmpVar)
	{
		cout << "New Order input (vo) elements are NOT matched with factor variables(var). Factor NOT change!" << endl;
		return -1;
	}
	for (int vCount = 0; vCount < varNum; vCount++)
	{
		intIter = find(var.begin(), var.end(), vo[vCount]);
		posMap.push_back(intIter - var.begin());
	}
	newCard = SubVector(card, posMap);
	for (int valCount = 0; valCount < valSize; valCount++)
	{
		IndexToAssignment(valCount+1, card, &assignment);
		assignment = SubVector(assignment, posMap);
		valIdx = AssignmentToIndex(assignment, newCard);
		newVal[valIdx-1] = val[valCount];
	}
	card = newCard;
	val = newVal;
	var = vo;
	return 0;
}

int Factor::ChangeVarOrder()
{
	vector <int> vo = var;
	sort(vo.begin(), vo.end());
	return ChangeVarOrder(vo);
}

//Print Out the Factor in a list form
void Factor::Show()
{
	string lineSep(30, '-');
	vector<int>::iterator iter1;
	int counter = 0;
	vector <int> rowRec(card.capacity(), 1);

	int recNum = accumulate(card.begin(), card.end(), 1,multiplies<int>());
	cout << fName << endl<<lineSep<<endl;
	for (iter1 = var.begin(); iter1 != var.end(); ++iter1)
		cout << *iter1 << '\t';
	cout << "value" << endl << lineSep<<endl;

	for (counter = 1; counter <= recNum; ++counter)
	{
		IndexToAssignment(counter, card, &rowRec);
		for (iter1 = rowRec.begin(); iter1 != rowRec.end(); ++iter1)
			cout << *iter1 << '\t';
		cout << val[counter-1] << endl;

	}



}

bool Factor::isempy()
{
	return var.empty();
}

std::vector<int> Factor::GetVar()
{
	// TODO: insert return statement here
	return var;
}

std::string Factor::GetFactorName()
{
	return fName;
}

std::vector<int> Factor::GetCard()
{
	// TODO: insert return statement here
	return card;
}

Factor Factor::ObserveEvidence(map<int, int> evidence)
{
	Factor newF;
	map <int, int> evPos = evidence; //evPos: Evidences Position in var;
	newF = *this;
	vector<int>::iterator itera1;
	vector <int> rowRec;
	vector <int> varPos;
	vector <int> assignment;
	vector <int> evAssignment;

	if (evidence.empty())
		return newF;

	for (auto it=evidence.begin();it!=evidence.end();++it)
	{
		itera1 = find(var.begin(), var.end(), it->first);
		if (itera1 != var.end())
		{
			varPos.push_back(itera1 - var.begin());
			evAssignment.push_back(it->second);
		}
	}
	
	if (varPos.empty())
	{
		cout << "No evidence found in current Var, original factor return!" << endl;
		return newF;
	}

	for (int counter = 0; counter < (int) val.size(); counter++)
	{
		IndexToAssignment(counter+1, card, &rowRec);
		assignment = SubVector(rowRec, varPos);
		if (assignment != evAssignment)
			newF.val[counter] = 0;
	}
	return newF;
}

Factor Factor::FactorMarginalization(std::vector<int> varList)
{
	if (varList.empty())
		return *this;
	Factor newF;
	vector <int> newVar;
	vector <int> newCard;
	vector <int> varPos;
	vector <int>::iterator it;
	vector <int> rowRec;
	vector <int> assignment;
	int idx;
	for (int counter = 0; counter < varLength; counter++)
	{
		it = find(varList.begin(), varList.end(), var[counter]);
		if (it == varList.end())
		{
			newCard.push_back(card[counter]);
			newVar.push_back(var[counter]);
			varPos.push_back(counter);
		}

	}

	int totalsize = newCard.size() + varList.size();
	if (totalsize > varLength)
	{
		cout << "Error: there is more than 1 variables are NOT in the Factor. Return this Factor" << endl;
		return *this;
	}
	if (newVar.empty())
	{
		//cout << "Error: Variables in V are all in Factor Var! Empty Factor Return!" << endl;
		return newF;
	}

	newF.FactorInit(newVar, newCard, 0);
	int valSize = val.size();
	for (int counter = 0; counter < valSize; counter++)
	{
		IndexToAssignment(counter + 1, card, &rowRec);
		assignment = SubVector(rowRec, varPos);
		idx = AssignmentToIndex(assignment, newF.GetCard())-1;
		newF.val[idx] = newF.val[idx] + val[counter];
	}


	return newF;
}

int Factor::FactorInit(vector<int> varList, vector<int> cardList, double value)
{
	int valSize = 0;
	var.clear();
	card.clear();
	val.clear();

	if (varList.size() != cardList.size())
	{
		cout << "Card List and Var List are NOT matched. Initialization Failed!" << endl;
		return -1;
	}
	varLength = varList.size();
	var = varList;
	card = cardList;
	valSize = accumulate(cardList.begin(), cardList.end(), 1, multiplies <int>());
	val.assign(valSize, value);
	fName = "Factor";
	return 0;
}

int Factor::FactorInit(std::vector<int> varList, std::vector<int> cardList, vector<double> value)
{
	FactorInit(varList, cardList, 1.0);
	if (value.size() != val.size())
	{
		cout << "Error: val size NOT matched! val is default value!" << endl;
		return -1;
	}
	else
		val = value;
	return 0;
}

void Factor::SetFactorName(std::string name)
{
	fName = name;
}

Factor::~Factor()
{
}

