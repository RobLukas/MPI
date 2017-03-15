#include <iostream>
#include <mpi.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <time.h>

void sendPackMatrix(double**, int, int);
void recvPackMatrix(double**, int, int);
void logFileFunction(int);
void printMatrix(double**, int);

using namespace std;

char* buf;
int packSize;
ofstream logFile;

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	logFileFunction(rank);

	double **matrixGenerated = new double *[size];
	double **matrixGet = new double *[size];

	srand((unsigned)time(NULL) + rank);
	for (int i = 0; i < size; i++, cout << endl) {
		matrixGenerated[i] = new double[size];
		for (int j = 0; j < size; j++, cout << " ") {
			double r = ((double)rand() / (RAND_MAX));
			matrixGenerated[i][j] = r;
		}
	}

	cout << "Matrix Generated: " << endl;
	printMatrix(matrixGenerated, size);
	cout << endl;

	int tPackSize;
	MPI_Pack_size(size, MPI_DOUBLE, MPI_COMM_WORLD, &tPackSize);
	packSize += tPackSize;
	cout << sizeof(double) << endl;
	cout << tPackSize << endl;
	cout << packSize << endl;

	
	if ((rank % 2) == 0) {
		sendPackMatrix(matrixGenerated, size, rank + 1);
		recvPackMatrix(matrixGet, size, rank + 1);
		//printMatrix(matrix, size);
	}
	else if ((rank % 2) == 1)
	{
		recvPackMatrix(matrixGet, size, rank - 1);
		sendPackMatrix(matrixGenerated, size, rank - 1);
	}

	cout << "Matrix Generated: " << endl;
	printMatrix(matrixGenerated, size);
	cout << endl;

	cout << "Matrix receive: " << endl;
	printMatrix(matrixGet, size);
	logFile.close();
	
	for (int i = 0; i < size; i++)
		delete[] matrixGenerated[i];
	delete[] matrixGenerated;

	for (int i = 0; i < size; i++)
		delete[] matrixGet[i];
	delete[] matrixGet;

	MPI_Finalize();
	return 0;
}

void sendPackMatrix(double** matrix, int size, int rank) {

	buf = new char[packSize];
	int position = 0;

	for (int i = 0; i < size; i++) {
		for (int j = 0; j < size; j++) {
			MPI_Pack(&i, 1, MPI_INT, buf, packSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&j, 1, MPI_INT, buf, packSize, &position, MPI_COMM_WORLD);
			MPI_Pack(&matrix[i][j], 1, MPI_DOUBLE, buf, packSize, &position, MPI_COMM_WORLD);
		}
	}
	MPI_Send(&packSize, 1, MPI_INT, rank, 321, MPI_COMM_WORLD);
	MPI_Send(buf, packSize, MPI_PACKED, rank, 123, MPI_COMM_WORLD);
	delete[] buf;
}

void recvPackMatrix(double** matrix, int size, int rank) {
	MPI_Status status;
	int position = 0, j, i;
	double elements;

	MPI_Recv(&packSize, 1, MPI_INT, rank, 321, MPI_COMM_WORLD, &status);
	buf = new char[packSize];
	MPI_Recv(buf, packSize, MPI_PACKED, rank, 123, MPI_COMM_WORLD, &status);

	//for (int i = 0; i < size; i++)
	//{
	//	for (int j = 0; j < size; j++) {
	//		MPI_Unpack(buf, packSize, &position, &elements, 1, MPI_DOUBLE, MPI_COMM_WORLD);
	//		matrix[i][j] = elements;
	//	}
	//}

	for (int k = 0; k < size; k++)
	{
		MPI_Unpack(buf, packSize, &position, &i, size, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buf, packSize, &position, &j, size, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buf, packSize, &position, &elements, size, MPI_DOUBLE, MPI_COMM_WORLD);
		matrix[i][j] = elements;
	}

	cout << elements;
	delete[] buf;
}

void logFileFunction(int rank) {
	fstream oss;
	logFile.open("log.proc_" + rank, fstream::out | fstream::trunc);
}

void printMatrix(double** matrix, int size) {
	for (int i = 0; i<size; i++) {
		for (int j = 0; j<size; j++) {
			logFile << std::fixed;
			logFile << matrix[i][j] << " ";
		}
		logFile << endl;
	}
}