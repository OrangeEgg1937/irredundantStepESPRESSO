#include <iostream>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

class PCN {
public:
	vector<int> Element;
	int NumOfDC = 0;

	// Termination Conditions 1: Does contain an all-DC cube or not
	bool isAllDC(int numberOfVariables) {
		return (NumOfDC == numberOfVariables);
	}
};

class ClubList {
public:
	vector<PCN> Cube;
	vector<unsigned int>VariablesPosCount;
	vector<unsigned int>VariablesNegCount;
	int NumberOfVariables = 0;
	int NumberOfCubes = 0;

	ClubList() {}
	ClubList(int numOfVar, int numOfCube) {
		// init the public member
		NumberOfVariables = numOfVar;
		NumberOfCubes = numOfCube;

		for (int i = 0; i < NumberOfVariables; i++)
		{
			VariablesPosCount.push_back(0);
			VariablesNegCount.push_back(0);
		}
	}

	// Tautology check
	// return -1 if cannot be determined, return 0 if false, return 1 if true
	int isTautology() {
		// Termination Conditions 1
		for (int i = 0; i < Cube.size(); i++)
		{
			if (Cube[i].isAllDC(NumberOfVariables)) return 1;
		}

		// Termination Conditions 2
		if (isUnate()) return 0;

		// Termination Conditions 3
		if (isSingleVarCubeBoth()) return 1;

		// Cannot determined
		return -1;
	}

private:
	// Termination Conditions 2: is unate and it does not contain an all-DC cube 
	// check the f is unate or not
	bool isUnate() {
		for (int i = 0; i < NumberOfVariables; i++)
		{
			// if both true and complement var appear (i.e both > 0), then it is not unate
			// stop the loop and return false
			if (VariablesPosCount[i] > 0 && VariablesPosCount[i] > 0)
			{
				return 0;
			}
		}
		return 1;
	}

	// Termination Conditions 3: single var cube appearing in both polarities
	bool isSingleVarCubeBoth() {
		bool hasBothPolarityCube = false;

		// first, we check whether there are exist a variables appearing in both polarity
		for (int i = 0; i < NumberOfVariables; i++)
		{
			if (VariablesNegCount[i] > 0 && VariablesPosCount[i] > 0)
			{

			}
		}
		return hasBothPolarityCube;
	}
};

ClubList readFile(char* path) {
	ifstream myfile;
	myfile.open(path);
	string data;

	// check file status
	if (myfile.is_open()) {
		// get the number of variables
		getline(myfile, data);
		int numberOfVariables = stoi(data);

		// get the number of cubes
		getline(myfile, data);
		int numberOfCubes = stoi(data);

		// create a ClubList instance
		ClubList F(numberOfVariables, numberOfCubes);

		while (getline(myfile, data))
		{
			// create a PCN instance
			PCN cube;

			// put the data into PNC cube
			for (int i = 0; i < F.NumberOfVariables; i++)
			{
				int code = stoi(string(1, data[i]));
				cube.Element.push_back(code);
				if (code == 0)
				{
					F.VariablesNegCount[i]++;
				}
				else if (code == 1) {
					F.VariablesPosCount[i]++;
				}
				else {
					cube.NumOfDC++;
				}
			}

			F.Cube.push_back(cube);
		}

		return F;
	}
	else
	{
		cout << "Error: Unable to open file!";
		exit(1);
	}
}

//int main(int argc, char* argv[])
//{
//	if (argc < 2 || argc > 3)
//	{
//		printf("usage: reduce <in_file> <out_file>");
//		exit(1);
//	}
//	char* inFile = argv[1];
//	char* outFile = argv[2];
//
//	std::cout << inFile << " " << outFile << "\n";
//
//	ClubList F = readFile(inFile);
//
//	return 0;
//}
