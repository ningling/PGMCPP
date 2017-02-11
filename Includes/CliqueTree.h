#pragma once
#include <vector>
#include "Factor.h"
class CliqueTree
{
public:
	CliqueTree();
	CliqueTree(std::vector <Factor> fList);
	CliqueTree(std::vector <Factor> fList, std::map <int, int> evidence);
	~CliqueTree();
	std::vector <std::vector <bool>> cliqueEdges;//Edges between Cliques
	std::vector <Factor> cliqueList;
	std::vector <std::vector <Factor>> msgCliques;

	int CreateCliqueTree();
	int CreateCliqueTree(std::vector <Factor> fList);
	int CreateCliqueTree(std::vector <Factor> fList, std::map <int, int> evidence);

	std::vector <Factor> ComputeExactMarginalsBP(bool isMax);
	std::vector <Factor> ComputeExactMarginalsBP(std::map<int, int> evidence, bool isMax);


private:
	std::vector <int> varList;
	std::vector <int> cardList;
	std::vector <Factor> factorsList;
	std::vector <int> factorInds;
	std::vector <std::vector<bool>> edges;//edges between variables
	std::vector <std::vector <int>> cliqueNodes;

	int CreateVarList();
	int CreateCardEdges();
	int EliminateVar(std::vector<Factor> *F, std::vector <std::vector<bool>> *edgesClique, int bestClique);
	void PruneTree();
	void ComputeInitialPotentials();
	bool ValidateAssignment(std::vector <int> assignment);
	std::vector <Factor> CliqueTreeCalibrate();
	std::vector <Factor> CliqueTreeMaxCalibrate();
	std::vector <int> GetNextClique();
};

