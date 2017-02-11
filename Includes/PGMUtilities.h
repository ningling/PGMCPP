#pragma once
#include <vector>
#include <iostream>

template <typename T>
std::vector <std::vector<T>> CreateMatrix(int m, int n, T initValue);

template<typename T>
inline std::vector<std::vector<T>> CreateMatrix(int m, int n, T initValue)
{
	std::vector <T> singleRow(n, initValue);
	std::vector <std::vector <T>> retVector(m, singleRow);
	return retVector;
}

template<typename T> 
void PrintPGMMatrix(std::vector <std::vector<T>> pgmMatrix);

template<typename T>
inline void PrintPGMMatrix(std::vector<std::vector<T>> pgmMatrix)
{
	int i, j;
	std::vector <int> varRow;
	for (i = 0; i < (int)pgmMatrix.size(); i++)
	{
		for (j = 0; j < (int)pgmMatrix[i].size(); j++)
		{
			std::cout << pgmMatrix[i][j] << '\t';
		}
		std::cout << '\n';
	}
}

template <typename T>
std::vector<T> SubVector(std::vector <T> input, std::vector <int> pos);

template<typename T>
inline std::vector<T> SubVector(std::vector<T> input, std::vector<int> pos)
{
	std::vector<T> result(pos.size());
	for (int counter = 0; counter < (int)pos.size(); counter++)
		result[counter] = input[pos.at(counter)];
	return result;
}

template<typename T>
inline std::vector<int> findValueFromVector(T value, std::vector<T> srcVec)
{
	int vecSize = srcVec.size();
	std::vector <int> retVec;
	for (int i = 0; i < vecSize; i++)
	{
		if (srcVec[i] == value)
			retVec.push_back(i);
	}
	return retVec;
}

template<typename T>
std::vector<int> findValueFromVector(T value, std::vector <T> srcVec);
