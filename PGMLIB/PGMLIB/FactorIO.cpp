#include "FactorUtilities.h"
#include "Factor.h"
#include "PGMUtilities.h"
#include <FactorIO.h>
#include <iostream>
#include <fstream>
#include <numeric>
#include <algorithm>
#include <string>

#define H5_BUILT_AS_DYNAMIC_LIB 1
#include "H5Cpp.h"

using namespace std;
using namespace H5;

std::vector<Factor> ImportFactors(std::string filename, std::string factorsetName, FactorFileType ftype)
{
	vector<Factor> factorset;

	switch (ftype)
	{
	case text:
		if (FactorsetFromText(filename, &factorset) != 0)
		{
			cout<< "Error: could NOT open " << filename << "\n";
			return factorset;
		}
		break;
	case bin:
		break;
	case mat7:
		break;
	case hdf5:
		if (FactorsetFromHDF5(filename, factorsetName, &factorset) != 0)
		{
			cout << "Error: Fail to read factors from file: " << filename << ". Empty set returned"<<endl;
			return factorset;
		}
		break;
	default:
		break;
	}
	return factorset;
}

//Get factor set data from HDF5 file. Assumed that the number of factors is under 100
int FactorsetFromHDF5(string filename, string factorsetRoot, vector<Factor> *factorset)
{
	const H5std_string FILE_NAME(filename);
	const H5std_string DATASET_ROOT(factorsetRoot);

	try
	{
		Exception::dontPrint();
		H5File file(FILE_NAME.c_str(), H5F_ACC_RDONLY);
		DataSet dataset;
		int dimsInt[2];

		int numFactors;
		string fID;
		
		vector <int> varList;
		vector <int> cardList;
		vector <double> valList;
		Factor factor;
		string factorname;
		//Get how many factors in the set;
		H5std_string datasetName=factorsetRoot;
		datasetName.append("/var/value/dims");
		dataset = file.openDataSet(datasetName.c_str());
		dataset.read(dimsInt, PredType::NATIVE_INT);
		numFactors = (dimsInt[0] > dimsInt[1]) ? dimsInt[0] : dimsInt[1];

		//cout << "There are " << numFactors << " factors" << endl;
		dataset.close();

		for (int fCount = 0; fCount < numFactors; fCount++)
		{
			fID = to_string(fCount);
			datasetName = factorsetRoot;
			if (fCount < 10&&numFactors>=10)
				datasetName.append("/var/value/_0");
			else
				datasetName.append("/var/value/_");
			datasetName.append(fID);
			datasetName.append("/value");
			dataset = file.openDataSet(datasetName.c_str());
			GetIntVectorFromHDF5(&dataset, &varList);
			dataset.close();

			//Get Card
			datasetName = factorsetRoot;
			if (fCount < 10&&numFactors>=10)
				datasetName.append("/card/value/_0");
			else
				datasetName.append("/card/value/_");
			datasetName.append(fID);
			datasetName.append("/value");
			dataset = file.openDataSet(datasetName.c_str());
			GetIntVectorFromHDF5(&dataset, &cardList);
			dataset.close();

			//Get Val
			datasetName = factorsetRoot;
			if (fCount < 10&&numFactors>=10)
				datasetName.append("/val/value/_0");
			else
				datasetName.append("/val/value/_");
			datasetName.append(fID);
			datasetName.append("/value");
			dataset = file.openDataSet(datasetName.c_str());
			GetDoubleVectorFromHDF5(&dataset, &valList);
			dataset.close();



			factor.FactorInit(varList, cardList, valList);
			factorname = "Factor ";
			factorname.append(fID);
			factor.SetFactorName(factorname);
			factorset->push_back(factor);

		}
		dataset.close();
		file.close();
		return 0;

	}
	catch (FileIException error)
	{
		error.printError();
		return -1;
	}
	catch (DataSetIException error)
	{
		error.printError();
		return -1;
	}
	catch (DataSpaceIException error)
	{
		error.printError();
		return -1;
	}
	catch (DataTypeIException error)
	{
		error.printError();
		return -1;
	}


	return 0;
}

int FactorsetFromText(std::string filename, std::vector<Factor>* factorset)
{
	Factor factor;
	factorset->clear();
	auto size = 0;
	string str;
	string strLine;
	int factorNum = 0;
	if (ifstream is{ filename,ios::binary | ios::ate })
	{
		size = is.tellg();
		str.assign(size, '\0');
		is.seekg(0);
		is.read(&str[0], size);
		//cout << str << "\n";
	}
	else
	{
		cout << "Error: could NOT open " << filename << "\n";
		return -1;
	}
	
	strLine.assign({ str[5] ,str[6] });
	factorNum=stoi(strLine);
	cout << "Totally we have " << factorNum << " factors\n";

	for (int count = 7; count < (int)size; count++)
	{
		switch (str[count])
		{
		case '\n':
			cout << "\\n" << endl;
			break;
		default:
			cout << str[count];

		}
	}

	return 0;
}


/*The functions Get<T>VectorFromHDF5 here are to extract data from the hdf5 file. We assume the data is just vector(x or y =1)*/
int GetDoubleVectorFromHDF5(H5::DataSet * dataset, std::vector<double>* vectorRet)
{
	DataType dType;
	size_t elementSize = 0;
	int vecLen = 0;
	double *buffer;
	
	dType = dataset->getDataType();
	elementSize = dType.getSize();
	vecLen = dataset->getInMemDataSize() / elementSize;
	buffer = new double[vecLen];
	dataset->read(buffer, PredType::NATIVE_DOUBLE);
	vectorRet->clear();
	for (int t = 0; t < vecLen; t++)
		vectorRet->push_back(buffer[t]);

	return 0;
}

int GetIntVectorFromHDF5(H5::DataSet * dataset, std::vector<int>* vectorRet)
{
	DataType dType;
	size_t elementSize = 0;
	int vecLen = 0;
	int *buffer;

	dType = dataset->getDataType();
	elementSize = dType.getSize();
	vecLen = dataset->getInMemDataSize() / elementSize;
	buffer = new int[vecLen];
	dataset->read(buffer, PredType::NATIVE_INT);
	vectorRet->clear();
	for (int t = 0; t < vecLen; t++)
		vectorRet->push_back(buffer[t]);

	return 0;
}


