/*TODO:
1. CreateCliqueTree: Need to do test on the case with evidence
*/
#include "CliqueTree.h"
#include <vector>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <numeric>
#include "PGMUtilities.h"
#include "Factor.h"
#include "FactorUtilities.h"

using namespace std;


CliqueTree::CliqueTree()
{
	cout << "Empty Factor set found. Need to specify factor set first!\n";
}

CliqueTree::CliqueTree(std::vector<Factor> fList)
{
	factorsList = fList;
	if (CreateCliqueTree() != 0)
	{
		cout << "Error: Clique Tree Init failed!!!" << endl;
	}
}

CliqueTree::CliqueTree(std::vector<Factor> fList, std::map<int, int> evidence)
{
	CreateCliqueTree(fList, evidence);
}


CliqueTree::~CliqueTree()
{
}

int CliqueTree::CreateCliqueTree()
{
	int varNum = 0;
	int fNum = factorsList.size();
	int bestClique = 0;
	int bestScore = 0;
	int edgeSize = 0;
	int edgeCount = 0;
	int score = 0;
	std::vector <std::vector<bool>> fEdges;

	vector <Factor> factorSet = factorsList;
	factorInds.assign(fNum, -1);

	//In case no init for factorList
	if (fNum == 0)
	{
		cout << "Error! No factor set. Please assign a factor set first!" << endl;
		return -1;
	}

	CreateVarList();
	varNum = varList.size();
	cardList.assign(varNum, 0);
	edges=CreateMatrix(varNum, varNum, false);
	CreateCardEdges();
	fEdges = edges;
	cliqueEdges = CreateMatrix(fNum, fNum, false);

	for (int varCount = 0; varCount < varNum; varCount++)
	{
		bestClique = 0;
		bestScore = varNum + 1;
		edgeSize = fEdges.size();
		for (edgeCount = 0; edgeCount < edgeSize; edgeCount++)
		{
			score = accumulate(fEdges[edgeCount].begin(), fEdges[edgeCount].end(),0);
			if (score > 0 && score < bestScore)
			{
				bestScore = score;
				bestClique = varList[edgeCount];
			}
		}
		if (EliminateVar(&factorSet,&fEdges, bestClique) != 0)
		{
			cout << "Error: Var Elimination failed!" << endl;
			return -1;
		}
		//cout << "Eliminate var " << bestClique << endl;

	}
	//Block Begin: This block is for debug, disable it when debug finish
	//PrintPGMMatrix(cliqueEdges);
	//Block End
	PruneTree();
	ComputeInitialPotentials();
	
	//Initialization of message matrix. msgClique[i][j] refer to message from i to j
	int nodeNum = cliqueList.size();
	Factor nullFactor;
	vector <Factor> nullMsg(nodeNum, nullFactor);
	msgCliques.assign(nodeNum, nullMsg);

	return 0;
}

int CliqueTree::CreateCliqueTree(std::vector<Factor> fList)
{
	factorsList.clear();
	factorsList = fList;
	return CreateCliqueTree();
}

int CliqueTree::CreateCliqueTree(std::vector<Factor> fList, std::map<int, int> evidence)
{
	factorsList.clear();
	for (auto factor : fList)
	{
		factor.ObserveEvidence(evidence);
		factorsList.push_back(factor);
	}
	return CreateCliqueTree();
}

std::vector<Factor> CliqueTree::CliqueTreeCalibrate()
{
	int cliqueNum = cliqueList.size();
	vector <Factor> calibratedCliques(cliqueNum);
	vector <int> nextij(2, -1);
	vector <int> msgVar;
	vector <int> nonmsgVar;
	vector <int> marginalVar;
	vector <int> iVar, jVar;
	vector <int> neighbors;
	map <int, int> ev;
	/*vector <vector<Factor>> unnormalizedMsg = msgCliques;
	Factor miuSepset;
	double miuValSepset;*/
	int i, j;
	nextij = GetNextClique();
	//Calibration and calculate messages
	while (nextij[0] != -1)
	{
		i = nextij[0];
		j = nextij[1];
		iVar = cliqueList[i].GetVar();
		jVar = cliqueList[j].GetVar();
		set_intersection(iVar.begin(), iVar.end(), jVar.begin(), jVar.end(), back_inserter(msgVar));
		neighbors = findValueFromVector(true, cliqueEdges[i]);
		msgCliques[i][j] = cliqueList[i];
		for (auto nb:neighbors)
		{
			if (nb != j)
				msgCliques[i][j] = msgCliques[i][j] * msgCliques[nb][i];
		}
		nonmsgVar = msgCliques[i][j].GetVar();
		sort(nonmsgVar.begin(), nonmsgVar.end());
		sort(msgVar.begin(), msgVar.end());
		set_difference(nonmsgVar.begin(), nonmsgVar.end(), msgVar.begin(), msgVar.end(), back_inserter(marginalVar));
		msgCliques[i][j] = msgCliques[i][j].FactorMarginalization(marginalVar);
		marginalVar.clear();
		msgVar.clear();
		
		/*unnormalizedMsg[i][j] = msgCliques[i][j];
		if (!unnormalizedMsg[j][i].isempy())
		{
			miuSepset = unnormalizedMsg[i][j] * unnormalizedMsg[j][i];
			miuValSepset = accumulate(miuSepset.val.begin(), miuSepset.val.end(), 0.0);
			cout << "miu[" << i +1<< "," << j+1 << "]=" << miuValSepset << endl;
		}*/

		msgCliques[i][j].val=Normalize(msgCliques[i][j].val);
		//cout << "Next Message is from Clique[" << nextij[0] + 1 << "] to Clique[" << nextij[1] + 1 << "]" << endl;
		nextij = GetNextClique();
	}



	//Calculate betas (belief)
	for (i = 0; i < cliqueNum; i++)
	{
		calibratedCliques[i] = cliqueList[i];
		neighbors = findValueFromVector(true, cliqueEdges[i]);
		for (auto nb : neighbors)
			calibratedCliques[i] = calibratedCliques[i] * msgCliques[nb][i];
	}

	return calibratedCliques;
}

std::vector<Factor> CliqueTree::CliqueTreeMaxCalibrate()
{
	return std::vector<Factor>();
}

std::vector<Factor> CliqueTree::ComputeExactMarginalsBP(bool isMax)
{
	map <int, int> evidence;
	return ComputeExactMarginalsBP(evidence,isMax);
}

std::vector<Factor> CliqueTree::ComputeExactMarginalsBP(map<int,int> evidence,bool isMax)
{
	if (factorsList.size() == 0)
	{
		cout << "Error: No factor list found. Please initial Clique Tree first!" << endl;
		exit(-1);
	}
	int varNum = varList.size();
	vector <int> cliqueVar;
	vector <Factor> retF(varNum);
	vector <Factor> cliquesTmp = cliqueList;
	vector <int>::iterator intIter;

	vector <Factor> calibratedCliques;

	if (!evidence.empty())
		for (int cCount = 0; cCount < (int)cliqueList.size(); cCount++)
			cliqueList[cCount].ObserveEvidence(evidence);
	calibratedCliques= isMax ? CliqueTreeMaxCalibrate() : CliqueTreeCalibrate();
	cliqueList = cliquesTmp;

	int cNum = calibratedCliques.size();
	for (int varCount = 0; varCount < varNum; varCount++)
	{
		for (int cliqueCount = 0; cliqueCount < cNum; cliqueCount++)
		{
			if (!retF[varCount].isempy())
				break;
			cliqueVar = calibratedCliques[cliqueCount].GetVar();
			intIter = find(cliqueVar.begin(), cliqueVar.end(), varList[varCount]);
			if (intIter!=cliqueVar.end())
				switch (isMax) {
				case true:
					break;
				case false:
					retF[varCount] = ComputeMarginal({ varList[varCount] }, { calibratedCliques[cliqueCount] }, evidence);
					retF[varCount].val = Normalize(retF[varCount].val);
				}
		}

	}
	return retF;
}

int CliqueTree::CreateVarList()
{
	int fNum = factorsList.size();
	vector <int> vars;
	vector <int> factorVar;
	varList.clear();
	for (int fCount = 1; fCount < fNum; fCount++)
	{
		factorVar = factorsList[fCount].GetVar();
		sort(factorVar.begin(), factorVar.end());
		set_union(varList.begin(), varList.end(), factorVar.begin(), factorVar.end(), back_inserter(vars));
		varList = vars;
		vars.clear();

	}
	return 0;
}

//Fill in initial Card and Edge values;
int CliqueTree::CreateCardEdges()
{
	int fNum = factorsList.size();
	vector <int>::iterator intIter;
	int i, j,posI,posJ;
	int varListLen;
	vector <int> factorVar;
	vector <int> factorCard;
	for (int fCount = 0; fCount < fNum; fCount++)
	{
		factorVar = factorsList[fCount].GetVar();
		factorCard = factorsList[fCount].GetCard();
		varListLen = factorVar.size();
		for (i = 0; i < varListLen; i++)
		{
			intIter = find(varList.begin(), varList.end(), factorVar[i]);
			posI = intIter - varList.begin();
			if (cardList[posI] != 0)
			{
				if (cardList[posI] != factorCard[i])//In case the input data has differnt card value for same variable
				{
					cout << "Error! card info for variable " << factorVar[i] << " NOT match!" << endl;
					return -1;
				}
			}
			else
				cardList[posI] = factorCard[i];
			for (j = 0; j < varListLen; j++)
			{
				intIter = find(varList.begin(), varList.end(), factorVar[j]);
				posJ = intIter - varList.begin();
				edges[posI][posJ] = true;
			}
		}
	}
	return 0;
}

int CliqueTree::EliminateVar(vector<Factor> *factorSet, std::vector <std::vector<bool>> *fEdges, int bestClique)
{
	vector <int> usedFactors;
	vector <int> nonusedFactors;
	vector <int> scope;
	vector <int> scopetmp;
	vector <Factor> newF;
	vector <int> varRec;
	vector <int>::iterator intIter;
	int edgeNum = fEdges->size();
	int i, j, posi, posj, bestCliquePos;
	intIter = find(varList.begin(), varList.end(), bestClique);
	bestCliquePos = intIter - varList.begin();
	

	int fNum = factorSet->size();
	vector <int> newmap(fNum, 0);

	for (i = 0; i < fNum; i++)
	{
		varRec = factorSet->at(i).GetVar();
		sort(varRec.begin(), varRec.end());
		intIter = find(varRec.begin(), varRec.end(), bestClique);
		if (intIter != varRec.end())
		{
			usedFactors.push_back(i);
			set_union(scope.begin(), scope.end(), varRec.begin(), varRec.end(), back_inserter(scopetmp));
			scope = scopetmp;
			sort(scope.begin(), scope.end());
			scopetmp.clear();
		}
		else
			nonusedFactors.push_back(i);
	}

	int scopeLen = scope.size();
	for (i = 0; i < scopeLen; i++)
	{
		intIter = find(varList.begin(), varList.end(), scope[i]);
		posi = intIter - varList.begin();
		for (j = 0; j < scopeLen; j++)
		{
			intIter = find(varList.begin(), varList.end(), scope[j]);
			posj = intIter - varList.begin();

			if (i != j)
			{
				fEdges->at(posi).at(posj)=true;
				fEdges->at(posj).at(posi) = true;
			}
		}
	}

	//Disconnecting best clique from the graph
	fEdges->at(bestCliquePos).assign(edgeNum, false);
	for (i = 0; i < edgeNum; i++)
		fEdges->at(i)[bestCliquePos] = false;

	for (i = 0; i < (int)nonusedFactors.size(); i++)
	{
		newF.push_back(factorSet->at(nonusedFactors[i]));
		newmap[nonusedFactors[i]]=i;
	}
	Factor newFactor;
	for (i = 0; i < (int)usedFactors.size(); i++)
		newFactor = newFactor*(factorSet->at(usedFactors[i]));
	newFactor = newFactor.FactorMarginalization({ bestClique });//TODO: newFactor might be just 1 element: {bestClique}. Need to avoid this situation.
	newF.push_back(newFactor);
	*factorSet = newF;

	int newC = cliqueNodes.size()+1;
	cliqueNodes.push_back(scope);
	factorInds[newC-1]=nonusedFactors.size();
	
	for (i = 0; i < newC-1; i++)
	{
		intIter = find(usedFactors.begin(), usedFactors.end(), factorInds[i]);
		if (intIter != usedFactors.end())
		{
			cliqueEdges[i][newC-1] = true;
			cliqueEdges[newC-1][i] = true;
			factorInds[i] = -1;
		}
		else
			if (factorInds[i] != -1)
				factorInds[i] = newmap[factorInds[i]];
	}
	//Block Begin: This block is for debug. Disable it after debug
	//cout << "Best Clique:" << bestClique << endl<<"Edges:\n";
	//PrintPGMMatrix(cliqueEdges);
	//Block End

	return 0;
}

void CliqueTree::PruneTree()
{
	vector <int> toRemove;
	vector <vector <int>> newNodes;
	vector <vector <bool>> newEdges;
	vector <int>::iterator intIter;
	vector <int> neighborsI;
	newNodes = cliqueNodes;
	newEdges = cliqueEdges;
	int cNum = newNodes.size();
	int nbCount = 0;
	int nbNum = 0;
	int nbConnect = 0;
	int j = -1;
	int edgeNum = cliqueEdges.size();//Assumed that cliqueEdges is a NxN matrix. dimensions are same size;
	int edgeCount = 0;
	for (int i = 0; i < cNum; i++)
	{
		//intIter = find(toRemove.begin(), toRemove.end(), i);
		//if (intIter != toRemove.end())
			//break;
		neighborsI = findValueFromVector(true, newEdges[i]);
		nbNum = neighborsI.size();
		if (!neighborsI.empty())
		{
			for (nbCount = 0; nbCount < nbNum; nbCount++)
			{
				j = neighborsI[nbCount];
				if (neighborsI[nbCount] == i)
				{
					cout << "Error: Clique#" << i << " neighbors should NOT include the clique itself" << endl;
					exit(-1);
				}
				intIter = find(toRemove.begin(), toRemove.end(), neighborsI[nbCount]);
				if (intIter != toRemove.end())
					break;

				if (includes(newNodes[j].begin(), newNodes[j].end(), newNodes[i].begin(), newNodes[i].end()))
				{
					for (nbConnect = 0; nbConnect < nbNum; nbConnect++)
					{
						if (nbConnect != nbCount)
						{
							newEdges[neighborsI[nbConnect]][j] = true;
							newEdges[j][neighborsI[nbConnect]] = true;
						}
					}
					newEdges[i].assign(edgeNum, false);
					for (edgeCount = 0; edgeCount < edgeNum; edgeCount++)
						newEdges[edgeCount][i] = false;
					toRemove.push_back(i);
				}
			}
		}
	}
	cliqueNodes.clear();
	cliqueEdges.clear();
	vector <bool> tmpEdge;
	for (int i = 0; i < cNum; i++)
	{
		intIter = find(toRemove.begin(), toRemove.end(), i);
		if (intIter == toRemove.end())
		{
			cliqueNodes.push_back(newNodes[i]);
			for (j = 0; j < cNum; j++)
			{
				intIter = find(toRemove.begin(), toRemove.end(), j);
				if (intIter == toRemove.end())
					tmpEdge.push_back(newEdges[i][j]);
			}
			cliqueEdges.push_back(tmpEdge);
			tmpEdge.clear();
		}
		//else
			//cout << "Line " << i << " Removed!" << endl;
	}
	//Block Begin: Debug usaged. Disable when debug finish.
	//cout << "New Edges:" << endl;
	//PrintPGMMatrix(cliqueEdges);
	//cout << "\nNodes to be removed:" << endl;
	//for (auto x : toRemove)
		//cout << x << ' ';
	//cout << endl;
	//Block End;
}

void CliqueTree::ComputeInitialPotentials()
{
	int nodeNum = cliqueNodes.size();
	int fNum = factorsList.size();
	vector <vector <int>> factor2NodesCandidates(fNum);
	vector <int> f2NAssignment(fNum,-1);
	vector <int> mapRec;
	int nCount, fCount,mCount;
	vector <int> factorVars;
	vector <int> mapFactorCards(fNum,0);
	vector <int> mapFactorVars;
	for (int i = 0; i < fNum; i++)
		mapFactorVars.push_back(i);
	Factor f2NMap;
	//factorVars.clear();
	cliqueList.assign(nodeNum, f2NMap); //Initialize cliqueList

	for (fCount = 0; fCount < fNum; fCount++)
	{
		for (nCount = 0; nCount < nodeNum; nCount++)
		{
			factorVars = factorsList[fCount].GetVar();
			sort(factorVars.begin(), factorVars.end());
			if (includes(cliqueNodes[nCount].begin(), cliqueNodes[nCount].end(), factorVars.begin(), factorVars.end()))
			{
				factor2NodesCandidates[fCount].push_back(nCount);
				mapFactorCards[fCount]++;
			}
		}

		if (mapFactorCards[fCount]==0)
		{
			cout << "Error: Factor " << fCount << " assigned to no Nodes" << endl;
			exit(-1);
		}
	}
	f2NMap.FactorInit(mapFactorVars, mapFactorCards, 0.0);

	for (mCount = 0; mCount < (int)f2NMap.val.size(); mCount++)
	{
		IndexToAssignment(mCount + 1, mapFactorCards, &mapRec);
		for (fCount = 0; fCount < fNum; fCount++)
			f2NAssignment[fCount] = factor2NodesCandidates[fCount][mapRec[fCount] - 1];
		if (ValidateAssignment(f2NAssignment))
			break;
	}

	for (fCount = 0; fCount < fNum; fCount++)
	{
		nCount = f2NAssignment[fCount];
		cliqueList[nCount] = cliqueList[nCount] * factorsList[fCount];
		cliqueList[nCount].ChangeVarOrder();
	}
	/*Debug Block Begin
	cout << "Right Assignment:[ ";
	for (auto x : f2NAssignment)
		cout << x << ' ';
	cout << ']'<<endl;
	string factorName;
	for (nCount = 0; nCount < nodeNum; nCount++)
	{
		factorName = "Clique #";
		factorName.append(to_string(nCount));
		cliqueList[nCount].SetFactorName(factorName);
		cliqueList[nCount].Show();
		cout << "********************************************************************" << endl;
	}
	*/
	//Debug Block End;
}

bool CliqueTree::ValidateAssignment(vector<int> assignment)
{
	int fNum = assignment.size();
	int nNum = cliqueNodes.size();
	vector <vector<int>> nodeFactors(nNum);
	vector <int> varTmp;
	vector <int> factorVars;
	int nodeNo;
	for (int fCount = 0; fCount < fNum; fCount++)
	{
		nodeNo = assignment[fCount];
		if ((nodeNo + 1) > nNum)
		{
			cout << "Error: node no. in assignment is greater than the size of clique nodes." << endl;
			exit(-1);
		}
		factorVars = factorsList[fCount].GetVar();
		sort(factorVars.begin(), factorVars.end());
		set_union(nodeFactors[nodeNo].begin(), nodeFactors[nodeNo].end(), factorVars.begin(), factorVars.end(), back_inserter(varTmp));
		nodeFactors[nodeNo] = varTmp;
		varTmp.clear();
	}

	for (int nCount = 0; nCount < nNum; nCount++)
		if (nodeFactors[nCount] != cliqueNodes[nCount])
			return false;

	return true;
}

std::vector<int> CliqueTree::GetNextClique()
{
	int i=-1, j=-1;
	int cNum = cliqueList.size();
	vector <int> retVal(2, -1);
	bool ready = false;
	vector <int> neighbors;
	int jCount, iCount,nbCount,downNb,jDown;
	for (iCount = 0; iCount < cNum; iCount++)
	{
		neighbors = findValueFromVector(true, cliqueEdges[iCount]);
		for (nbCount = 0; nbCount < (int)neighbors.size(); nbCount++)
		{
			jCount = neighbors[nbCount];
			if (msgCliques[iCount][jCount].isempy())//Find the message has NOT been passed. 
			{
				ready = true;
				for (downNb = 0; downNb < (int)neighbors.size(); downNb++)
				{
					if (downNb != nbCount)
					{
						jDown = neighbors[downNb];
						if (msgCliques[jDown][iCount].isempy()) //Find the readiness of the message to be pass: all messages to i except j should be ready
						{
							ready = false;
							break;
						}
						else
							ready = true;
					}
				}
				if (ready)
				{
					j = jCount;
					break;
				}
			}
		}
		if (ready)
		{
			retVal[0] = iCount;
			retVal[1] = j;
			return retVal;
		}
	}
	//cout << "No available message to be passed found!" << endl;
	return retVal;
}
