#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <array>

using std::stringstream;
using std::string;
using std::ifstream;
using std::ofstream;
using std::istringstream;
using std::cerr;
using std::cout;
using std::endl;

void grab(string infile, string outfile);

const int REPLICA_NUMBER = 16;
const int PE_MIN = -57100;
const int PE_MAX = -51700;
const int PE_BIN_STEP = 100;
const int PE_BIN_NUMBER = (PE_MAX - PE_MIN) / PE_BIN_STEP + 1;

//command line ./REMD.out energy_*
int main(int argc, char** argv)
{
	stringstream ss;
	ss << argv[1];
	string name;
	ss >> name;
	ss.clear();

	//grab data from *.xvg file and write to *.txt
	for (int i = 0; i < REPLICA_NUMBER; i++)
	{
		ss << i;
		string number;
		ss >> number;
		ss.clear();
		string infile = name + number + ".xvg";
		string outfile = name + number + ".txt";
		grab(infile, outfile);
	}

	//calculate distribution
	//generate dynamic array
	int** distribution;
	distribution = new int*[REPLICA_NUMBER];
	for (int i = 0; i < REPLICA_NUMBER; i++) {
		distribution[i] = new int[PE_BIN_NUMBER] {};
	}
	for (int i = 0; i < REPLICA_NUMBER; i++)
	{
		ss << i;
		string number;
		ss >> number;
		ss.clear();
		string infile = name + number + ".txt";
		ifstream fi;
		fi.open(infile);
		if (!fi) {
			cerr << "Error opening " << infile << " for input!\n";
			exit(-1);
		}
		double data;
		while (fi >> data)
		{
			int index = ((int)data - PE_MIN) / PE_BIN_STEP;
			distribution[i][index]++;
		}
		fi.close();
	}
	ofstream fo;
	fo.open("distribution.txt");
	for (int j = 0; j < PE_BIN_NUMBER; j++) {
		for (int i = 0; i < REPLICA_NUMBER; i++)
		{
			fo << distribution[i][j] << "\t";
		}
		fo << endl;
	}
	fo.close();
	//delete dynamic array
	for (int i = 0; i < REPLICA_NUMBER; i++) {
		delete[] distribution[i];
	}
	delete[] distribution;
	return 0;
}

void grab(string infile, string outflile) {
	ofstream fo;
	fo.open(outflile);
	ifstream fi;
	fi.open(infile);
	if (!fi)
	{
		cerr << "Error opening " << infile << " for input!\n";
		exit(-1);
	}
	string line;
	double data;
	double Emin = 0.0;
	double Emax = 0.0;
	while (getline(fi, line))
	{
		istringstream istream(line);
		string temp;
		istream >> temp;
		if (isdigit(temp[0])) {
			istream >> data;
			fo << data << endl;
			if (data < Emin || Emin == 0.0)
				Emin = data;
			if (data > Emax || Emax == 0.0)
				Emax = data;
		}
		istream.clear();
	}
	cout << "Emin = " << Emin << " , Emax = " << Emax << endl;
	fi.close();
	fo.close();
}
