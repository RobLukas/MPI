#include <iostream>
#include <mpi.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <time.h>

void sendPackMatrix(double**, int);
void recvPackMatrix(double**, int);
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

	double **matrix = new double *[size];

	srand((unsigned)time(NULL) + rank);
	cout << "Processor number: " << rank << endl;
	for (int i = 0; i < size; i++, cout << endl) {
		matrix[i] = new double[size];
		for (int j = 0; j < size; j++, cout << " ") {
			double r = ((double)rand() / (RAND_MAX));
			matrix[i][j] = r;
			cout << matrix[i][j] << " ";
		}
	}

	int tPackSize;
	MPI_Pack_size(size * size, MPI_DOUBLE, MPI_COMM_WORLD, &tPackSize);
	packSize = tPackSize;
	
	if (rank % 2 == 0) {
		recvPackMatrix(matrix, size);
		//printMatrix(matrix, size);
		for (int i = 0; i < size; i++, cout << endl) {
			for (int j = 0; j < size; j++, cout << " ") {
				cout << matrix[i][j] << " ";
			}
		}
	}
	else if (rank % 2 == 1)
	{
		sendPackMatrix(matrix, size);
		//printMatrix(matrix, size);
	} 

	cout << endl;

	for (int i = 0; i < size; i++)
		delete[] matrix[i];
	delete[] matrix;

	logFile.close();

	MPI_Finalize();
	return 0;
}

void sendPackMatrix(double** matrix, int size) {

	buf = new char[packSize];
	int position = 0;

	for (int i = 0; i < size - 1; i++) {
		for (int j = 0; j < size - 1; j++) {
			MPI_Pack(&matrix[i][j], 1, MPI_DOUBLE, buf, packSize, &position, MPI_COMM_WORLD);
		}
	}
	MPI_Send(buf, packSize, MPI_PACKED, 0, 200, MPI_COMM_WORLD);
	delete[] buf;
}

void recvPackMatrix(double** matrix, int size) {
	MPI_Status status;
	int position = 0, i, j;
	double elements;

	buf = new char[packSize];
	MPI_Recv(buf, packSize, MPI_PACKED, 1, 200, MPI_COMM_WORLD, &status);

	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++) {
			MPI_Unpack(buf, packSize, &position, &elements, 1, MPI_DOUBLE, MPI_COMM_WORLD);
			matrix[i][j] = elements;
		}
	}

	//for (int k = 0; k < size; k++)
	//{
	//	MPI_Unpack(buf, packSize, &position, &i, 1, MPI_INT, MPI_COMM_WORLD);
	//	MPI_Unpack(buf, packSize, &position, &j, 1, MPI_INT, MPI_COMM_WORLD);
	//	MPI_Unpack(buf, packSize, &position, &elements, 1, MPI_DOUBLE, MPI_COMM_WORLD);
	//	matrix[i][j] = elements;
	//}
	delete[] buf;
}

void logFileFunction(int rank) {
	ostringstream oss;
	oss << "log.proc_" << rank;
	logFile.open((oss.str().c_str()));
}

void printMatrix(double** matrix, int size) {
	logFile << "matrix = " << endl;
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			logFile << matrix[i][j] << " ";
		}
		logFile << endl;
	}
}