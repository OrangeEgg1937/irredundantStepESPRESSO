#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstdlib>
#include <set>
#include <algorithm>

// Global variable to stop the recursive function
bool GoboalStop = false;

class PCN {
public:
	std::vector<int> Element;
	int NumOfDC = 0;
	int NumOfPOS = 0;
	int NumOfNEG = 0;

	// Termination Conditions 1: Does contain an all-DC cube or not
	bool isAllDC(int numberOfVariables) {
		return (NumOfDC == numberOfVariables);
	}

	void dropElement(int index) {
		// update the number of POS/NEG/DC
		if (Element[index] == 0) NumOfNEG--;
		else if (Element[index] == 1) NumOfPOS--;
		else NumOfDC--;

		// drop the element
		Element.erase(Element.begin() + index);
	}

	void UpdateElement(int index, int value) {
		// update the number of POS/NEG/DC
		if (Element[index] == 0) NumOfNEG--;
		else if (Element[index] == 1) NumOfPOS--;
		else NumOfDC--;

		// update the element
		Element[index] = value;

		// update the number of POS/NEG/DC
		if (Element[index] == 0) NumOfNEG++;
		else if (Element[index] == 1) NumOfPOS++;
		else NumOfDC++;
	}	
};

class ClubList {
public:
	std::vector<PCN> Cube;
	std::vector<unsigned int>VariablesPosCount;
	std::vector<unsigned int>VariablesNegCount;
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

	void dropCube(int index) {
		// update the number of POS/NEG/DC
		for (int i = 0; i < NumberOfVariables; i++)
		{
			if (Cube[index].Element[i] == 0) VariablesNegCount[i]--;
			else if (Cube[index].Element[i] == 1) VariablesPosCount[i]--;
		}

		// update the number of cubes
		NumberOfCubes--;

		// drop the cube
		Cube.erase(Cube.begin() + index);
	}

	// Cofactor function, bool cofactorType: 1 for positive cofactor, 0 for negative cofactor
	static ClubList Cofactor(ClubList F, int varIndex, bool cofactorType) {
		// create a new ClubList instance
		ClubList Fx = F;

		// loop through the cube
		for (int i = 0; i < Fx.NumberOfCubes; i++)
		{
			// if the cube is 2 (DC), then continue to the next cube
			if (Fx.Cube[i].Element[varIndex] == 2) continue;

			// if the cube is equal to the cofactor type, then change to 2 (DC)
			// e.g: if the cofactor type is 1 (+ve) and the value is 1, then change 1 to 2
			if (Fx.Cube[i].Element[varIndex] == cofactorType)
			{
				// change the value to 2 (DC)
				Fx.Cube[i].UpdateElement(varIndex, 2);
			}
			else // if the cube is not equal to the cofactor type, then drop the cube
			{
				Fx.dropCube(i);
				i--; // since the cube is dropped, then the index should be decreased by 1 as the cube is shifted
			}
		}

		// update the positive/negative count
		Fx.VariablesPosCount[varIndex] = 0;
		Fx.VariablesNegCount[varIndex] = 0;

		return Fx;
	}

private:
	// Termination Conditions 2: is unate and it does not contain an all-DC cube 
	// check the f is unate or not
	bool isUnate() {
		for (int i = 0; i < NumberOfVariables; i++)
		{
			// if both true and complement var appear (i.e both > 0), then it is not unate
			// stop the loop and return false
			if (VariablesPosCount[i] > 0 && VariablesNegCount[i] > 0)
			{
				return 0;
			}
		}
		return 1;
	}

	// Termination Conditions 3: single var cube appearing in both polarities
	bool isSingleVarCubeBoth() {
		bool hasBothPolarityCube = false;
		// create a set to store the not visit row
		std::vector<int> notVisitRow;
		for (int i = 0; i < NumberOfCubes; i++)
		{
			notVisitRow.push_back(i);
		}

		// create a set to store the single var cube column index
		std::vector<int> singleVarCubeColIndex;

		// Step 1: check if there is a var appearing in both polarities
		for (int i = 0; i < NumberOfVariables; i++)
		{
			if (VariablesNegCount[i] > 0 && VariablesPosCount[i] > 0)
			{
				hasBothPolarityCube = true;
				singleVarCubeColIndex.push_back(i);
			}
		}

		// Step 2: if there is a single var cube appearing in both polarities, then check if there is a row that contains all the single var cube
		if (hasBothPolarityCube)
		{
			// 2.1: loop through the var column index
			for (int i = 0; i < singleVarCubeColIndex.size(); i++)
			{
				// create variables to check the +ve and -ve polarity is both valid or not
				bool isPosValid = false;
				bool isNegValid = false;

				// 2.2: loop through the not visit row
				for (int j : notVisitRow)
				{
					// 2.2.1: check the value of the var is 2 or not
					if (Cube[j].Element[singleVarCubeColIndex[i]] == 2)
					{
						// continue to the next row
						continue;
					}
					else // 2.2.2: then it is either 0 or 1
					{
						// 2.2.3: check is the +ve or -ve polarity is valid or not (signle var cube)
						if (Cube[j].Element[singleVarCubeColIndex[i]] == 0
							&& Cube[j].NumOfNEG == 1
							&& Cube[j].NumOfPOS == 0)
						{
							isNegValid = true;
						}
						else if (Cube[j].Element[singleVarCubeColIndex[i]] == 1
							&& Cube[j].NumOfNEG == 0
							&& Cube[j].NumOfPOS == 1)
						{
							isPosValid = true;
						}

						// Drop the row from the not visit row as it is already visited
						notVisitRow.erase(remove(notVisitRow.begin(), notVisitRow.end(), j), notVisitRow.end());
					}
				}

				// if both +ve and -ve polarity is valid, then return true
				if (isPosValid && isNegValid)
				{
					return true;
				}
			}
		}

		// if there is no single var cube appearing in both polarities, then return false
		return false;
	}
};

// read the file and put the data into ClubList instance
ClubList readFile(char* path) {
	std::ifstream myfile;
	myfile.open(path);
	std::string data;

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
				int code = std::stoi(std::string(1, data[i]));
				cube.Element.push_back(code);
				if (code == 0)
				{
					F.VariablesNegCount[i]++;
					cube.NumOfNEG++;
				}
				else if (code == 1) {
					F.VariablesPosCount[i]++;
					cube.NumOfPOS++;
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
		std::cout << "Error: Unable to open file!";
		exit(1);
	}
}

// save the file to the path
void saveFile(ClubList F, char* path) {
	// create a file
	std::ofstream outputFile(path);

	if (outputFile.is_open())
	{
		// write the number of variables and the number of cubes
		outputFile << F.NumberOfVariables << "\n";
		outputFile << F.Cube.size() << "\n";

		// write the cubes
		for (int i = 0; i < F.Cube.size(); i++)
		{
			for (int j = 0; j < F.NumberOfVariables; j++)
			{
				outputFile << F.Cube[i].Element[j];
			}
			outputFile << "\n";
		}

		outputFile.close();
	}
	else
	{
		std::cout << "Error: Unable to open file!";
		exit(1);
	}
}

// sub-function to find the most binate variable (for URPTatologyCheck)
int FindMostBinateVariable(ClubList F) {
	// init the variables
	int index = 0;
	int currMin = F.NumberOfCubes + 1;

	for (int i = 0; i < F.NumberOfVariables; i++)
	{
		// if the variable is binate
		if (F.VariablesNegCount[i] > 0 && F.VariablesPosCount[i] > 0)
		{
			int diff = abs((int)(F.VariablesPosCount[i] - F.VariablesNegCount[i]));

			// if the number of cubes is less than the current min, then update the index and the current min
			if ((diff) < currMin)
			{
				index = i;
				currMin = diff;
			}
		}
	}

	return index;
}

// implement URPTatologyCheck
bool URPTatologyCheck(ClubList F) {
	// if the GoboalStop is true, then stop the recursive function
	if (GoboalStop) return false;

	// process a tautology check
	int check = F.isTautology();

	// if it can be determined, then return the result
	if (check == 1) return true;
	if (check == 0) {
		GoboalStop = true;
		return false;
	}

	// if it cannot be determined, factoring the cube and check again
	ClubList F_POS;
	ClubList F_NEG;

	// find ¡§most binate¡¨ variable
	int varIndex = FindMostBinateVariable(F);

	// cofactor the cube
	F_POS = ClubList::Cofactor(F, varIndex, 1);
	F_NEG = ClubList::Cofactor(F, varIndex, 0);

	return URPTatologyCheck(F_POS) && URPTatologyCheck(F_NEG);
}

// For debugging
void printClubList(ClubList F) {
	for (int i = 0; i < F.Cube.size(); i++)
	{
		for (int j = 0; j < F.Cube[i].Element.size(); j++)
		{
			std::cout << F.Cube[i].Element[j] << " ";
		}
		std::cout << "\n";
	}
}

// implement Redundant Cube Removal
ClubList RCR(ClubList F) {
	// We store the index of the cube in the F, REC, TRC
	std::set<int> F_origin;
	std::set<int> F_notE;
	std::vector<unsigned int> posCount(F.NumberOfVariables, 0);
	std::vector<unsigned int> negCount(F.NumberOfVariables, 0);

	// Define a set for Relatively Essential Cube
	std::set<int> REC;

	// Define a set for Totally Redundant Cube
	std::set<int> TRC;

	// 2.1: Stage 1: Find the Relatively Essential Cube - E
	for (int i = 0; i < F.NumberOfCubes; i++)
	{
		F_origin.insert(i); // at the same time, we initialize the F_original in here
		F_notE.insert(i); // at the same time, we initialize the F_notE in here (F_notE = F \ E)

		ClubList Fc = F;
		PCN c = F.Cube[i];

		Fc.dropCube(i);

		// Since Fc is not depend on the cube c, then we can check if Fc is a tautology or not
		for (int j = 0; j < F.NumberOfVariables; j++)
		{
			if(c.Element[j] == 2) continue;
			Fc = ClubList::Cofactor(Fc, j, c.Element[j]);
		}

		// Check If a cube c is not contained by F \ {c} by URPTatologyCheck
		if (!URPTatologyCheck(Fc))
		{
			REC.insert(i);
			F_notE.erase(i);
			for (int j = 0; j < F.NumberOfVariables; j++)
			{
				if (c.Element[j] == 1) {
					posCount[j]++;
				}
				else if (c.Element[j] == 0) {
					negCount[j]++;
				}
			}
		}

		// reset the GoboalStop
		GoboalStop = false;
	}

	// Define a set T where T = F - E (T = F_origin - REC)
	// create a cublist T for relatively essential cubes
	ClubList E;
	E.NumberOfVariables = F.NumberOfVariables;
	E.NumberOfCubes = REC.size();
	E.VariablesPosCount = posCount;
	E.VariablesNegCount = negCount;
	for (int i = 0; i < F.NumberOfCubes; i++)
	{
		// if the cube is in REC, then add it to T
		if (REC.find(i) != REC.end())
		{
			E.Cube.push_back(F.Cube[i]);
		}
	}

	// 2.2: Stage 2: Find the Totally Redundant Cube
	for (int cube : F_notE) // for each cube c in F \ E
	{
		ClubList Fc = E;
		PCN c = F.Cube[cube];

		// Since Fc is not depend on the cube c, then we can check if Fc is a tautology or not
		for (int j = 0; j < F.NumberOfVariables; j++)
		{
			if(c.Element[j] == 2) continue;
			Fc = ClubList::Cofactor(Fc, j, c.Element[j]);
		}

		// Check Fc is empty or null, if yes, then c is not TRC, skip it
		if (Fc.NumberOfCubes == 0) continue;

		// Check If a cube c is contained by F \ E by URPTatologyCheck
		if (URPTatologyCheck(Fc))
		{
			TRC.insert(cube);
		}

		// reset the GoboalStop
		GoboalStop = false;
	}	

	// Build the result, Fr = F - TRC
	std::set<int> Fr;
	std::set_difference(F_origin.begin(), F_origin.end(), TRC.begin(), TRC.end(), inserter(Fr, Fr.begin()));

	ClubList result;
	for (int i : Fr)
	{
		result.Cube.push_back(F.Cube[i]);
	}

	return result;
}

int main(int argc, char* argv[])
{
	if (argc < 2 || argc > 3)
	{
		printf("usage: reduce <in_file> <out_file>");
		exit(1);
	}
	char* inFile = argv[1];
	char* outFile = argv[2];

	std::cout << "Processing file...(in:" << inFile << ", out:" << outFile << ")\n";

	ClubList F = readFile(inFile);
	ClubList reducedF = RCR(F);
	reducedF.NumberOfVariables = F.NumberOfVariables;

	// save the file
	saveFile(reducedF, outFile);

	std::cout << "Done, File has been processed and result saved successfully!\n";

	return 0;
}
