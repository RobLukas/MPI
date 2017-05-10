#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <cmath>
#include <stdio.h>
#define DEBUG

using namespace std;

void sendDataByMaster();
void receiveDataFromMaster();
double integrate(double, double, int);
double evaluateFOfX(double);

int packSize;
char *packBuff;
int packPosition;
int sizeVec;

int sizeDegreeIntegration = 2;
int sizeDataVec = 0;

int startPosition = 12;
string line;
double *dataVec;
int *degreeIntegration = new int[2];

int main(int argc, char** argv) {

	MPI_Init(&argc, &argv);

	int division, rest;
	int myNumberOfSubintervals;
	int myFirstMidPoint;
	double l, u;

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0)
	{
		ifstream fileData(argv[1]);
		if (!fileData.good())
		{
			cout << "Can't open file!" << endl;
			MPI_Finalize;
			return 0;
		}

		int count = 0, l = 0;
		while (getline(fileData.seekg(startPosition, ios::cur), line))
		{
			double var;
			istringstream ss(line);
			while (ss >> var)
			{
				if (count == 0)
				{
					degreeIntegration[0] = var;
					dataVec = new double[degreeIntegration[0] + 3];
					sizeDataVec = degreeIntegration[0] + 3;
				}
				else if (count == 3)
				{
					degreeIntegration[1] = var;
				}
				else
				{
					dataVec[l] = var;
					l++;
				}
			}
			count++;
		}
		sendDataByMaster();
	}
	else
	{
		receiveDataFromMaster();
	}

	//myNumberOfSubintervals liczba interwa³ów podrzêdnych
	//myFirstMidPoint pierwszy punkt
	division = degreeIntegration[1] / size;
	rest = degreeIntegration[1] % size;
	if (rank < rest) {
		myNumberOfSubintervals = division + 1;
		myFirstMidPoint = rank*(division + 1);
	}
	else {
		myNumberOfSubintervals = division;
		myFirstMidPoint = rest*(division + 1) + (rank - rest) * division;
	}

	double h1 = ((dataVec[degreeIntegration[0] + 2]) - (dataVec[degreeIntegration[0] + 1])) / degreeIntegration[1];
	double aT = (dataVec[degreeIntegration[0] + 1]);

	l = aT + (h1 * myFirstMidPoint);
	u = aT + (h1 * (myFirstMidPoint + myNumberOfSubintervals));

	double partOfIntegrate = integrate(l, u, myNumberOfSubintervals);

	double reduce = 0;
	MPI_Reduce(&partOfIntegrate, &reduce, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		double A, B;

		for (int i = 0; i <= degreeIntegration[0]; i++)
		{
			A += (dataVec[i] * pow(dataVec[degreeIntegration[0] + 1], i + 1)) / (i + 1);
		}

		for (int i = 0; i <= degreeIntegration[0]; i++)
		{
			B += (dataVec[i] * pow(dataVec[degreeIntegration[0] + 2], i + 1)) / (i + 1);
		}
		cout << "Value: " << setprecision(15) << reduce << endl;
		cout << "Value analitycally: " << setprecision(15) << B - A << endl;
	}

	delete[] degreeIntegration;
	delete[] dataVec;

	cout << endl;

	MPI_Finalize();
	return 0;
}

double integrate(double a, double b, int m)
{
	double x =0;
	double f_of_x=0;
	double h = (b - a) / m;
	double integral = 0.0;
	for (int i = 0; i <= m; i++)
	{
		x = a + i * h;
		f_of_x = evaluateFOfX(x);
		if ((i == 0) || (i == m))
			integral += 0.5 * f_of_x;
		else
			integral += f_of_x;
	}
	integral *= h;
	return integral;
}

void sendDataByMaster() {
	int tPackSize;
	packPosition = 0;
	
	MPI_Pack_size(sizeDegreeIntegration, MPI_INT, MPI_COMM_WORLD, &tPackSize);
	packSize = tPackSize;
	MPI_Pack_size(sizeDataVec, MPI_DOUBLE, MPI_COMM_WORLD, &tPackSize);
	packSize += tPackSize;

	packBuff = new char[packSize];
	MPI_Pack(&degreeIntegration[0], sizeDegreeIntegration, MPI_INT, packBuff, packSize, &packPosition, MPI_COMM_WORLD);
	MPI_Pack(&dataVec[0], sizeDataVec, MPI_DOUBLE, packBuff, packSize, &packPosition, MPI_COMM_WORLD);

	MPI_Bcast(&packSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(packBuff, packSize, MPI_PACKED, 0, MPI_COMM_WORLD);
	packPosition = 0;

	delete[] packBuff;
}

void receiveDataFromMaster() {
	MPI_Bcast(&packSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	packBuff = new char[packSize];
	MPI_Bcast(packBuff, packSize, MPI_PACKED, 0, MPI_COMM_WORLD);

	packPosition = 0;
	MPI_Unpack(packBuff, packSize, &packPosition, &degreeIntegration[0], sizeDegreeIntegration, MPI_INT, MPI_COMM_WORLD);
	dataVec = new double[degreeIntegration[0] + 3];
	sizeDataVec = degreeIntegration[0] + 3;

	MPI_Unpack(packBuff, packSize, &packPosition, &dataVec[0], sizeDataVec, MPI_DOUBLE, MPI_COMM_WORLD);
	delete[] packBuff;
}

double evaluateFOfX(double x)
{
	double sum = 0;
	for (int i = 0; i <= degreeIntegration[0]; i++)
	{
		sum += dataVec[i] * pow(x, i);
	}
	return sum;
}