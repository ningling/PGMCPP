#pragma once
#include <vector>
#include <map>

class Factor
{
	//friend Factor operator==(Factor A, Factor B);
	//friend Factor operator*(Factor A, Factor B);
public:
	std::vector <double> val;
	Factor();
	Factor(std::vector <int> varList, std::vector <int> cardList, double value) { FactorInit(varList,cardList,value); }
	Factor(std::vector <int> varList, std::vector <int> cardList, std::vector <double> value);

	int FactorInit(std::vector <int> varList, std::vector <int> cardList, double value);
	int FactorInit(std::vector <int> varList, std::vector <int> cardList, std::vector <double> value);

	int GetValueOfAssignment(std::vector <int> A, std::vector <int> vo, double *value);
	int GetValueOfAssignment(std::vector <int> A, double *value);
	int SetValueOfAssignment(double value, std::vector <int> A);
	int SetValueOfAssignment(double value, std::vector <int> A, std::vector <int> vo);
	int ChangeVarOrder(std::vector <int> vo);
	int ChangeVarOrder();
	
	void Show();
	bool isempy();
	//bool GetVar(std::vector <int> &vecVar);
	std::vector <int> GetVar();
	std::string GetFactorName();
	std::vector <int> GetCard();
	void SetFactorName(std::string name);

	Factor ObserveEvidence(std::map <int, int> evidence);
	Factor FactorMarginalization(std::vector<int> varList);

	~Factor();
private:
	int varLength = 0;
	std::vector <int> var;
	std::vector <int> card;
	std::string fName;

};



