#pragma once
#include <vector>
#include "Factor.h"
#define H5_BUILT_AS_DYNAMIC_LIB 1

#include "H5Cpp.h"
//#ifndef H5_NO_NAMESPACE
//using namespace H5;
//#endif

enum FactorFileType
{
	text,bin,mat7,hdf5
};
std::vector <Factor> ImportFactors(std::string filename, std::string FactorsetName, FactorFileType ftype);
int FactorsetFromHDF5(std::string filename, std::string FactorsetName, std::vector <Factor> *factorset);
int FactorsetFromText(std::string filename, std::vector <Factor> *factorset);

//The following functions should be in utilities.cpp
int GetDoubleVectorFromHDF5(H5::DataSet *dataset, std::vector <double> *vector);
int GetIntVectorFromHDF5(H5::DataSet *dataset, std::vector <int> *vector);