#include <iostream>
#include <mpi.h>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <random>

//void sendPackMatrix(double**, int, int);
//void recvPackMatrix(double**, int, int);
void logFileFunction(int);
void printMatrix(double*, int);

using namespace std;

fstream logFile;

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);

	int rank, size;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	//double **matrixGenerated2D = new double *[size];
	double *matrixGenerated = new double[size*size];
	double *matrixGet = new double [size*size];

	srand((unsigned)time(NULL) + rank*rank + size);
	int k = 0;
	for (int i = 0; i < size; i++) {
		//matrixGenerated2D[i] = new double[size];
		for (int j = 0; j < size; j++) {
			double r = ((double)rand() / (RAND_MAX));
			//matrixGenerated2D[i][j] = r;
			//matrixGenerated[k++] = matrixGenerated2D[i][j];
			matrixGenerated[size*i + j] = r;
		}
	}

	logFileFunction(rank);

	MPI_Status status;
	if ((rank % 2) == 0) {
		MPI_Send(matrixGenerated, size*size, MPI_DOUBLE, rank + 1, 123, MPI_COMM_WORLD);
		MPI_Recv(matrixGet, size*size, MPI_DOUBLE, rank + 1, 123, MPI_COMM_WORLD, &status);
	}
	else
	{
		MPI_Recv(matrixGet, size*size, MPI_DOUBLE, rank - 1, 123, MPI_COMM_WORLD, &status);
		MPI_Send(matrixGenerated, size*size, MPI_DOUBLE, rank - 1, 123, MPI_COMM_WORLD);
	}


	if ((rank % 2) == 0)
		logFile << "Generate matrix and sent to process: " << rank + 1 << endl;
	else
		logFile << "Generate matrix and sent to process: " << rank - 1 << endl;

	printMatrix(matrixGenerated, size);

	if ((rank % 2) == 0)
		logFile << "Receive matrix from process: " << rank + 1 << endl;
	else
		logFile << "Receive matrix from process: " << rank - 1 << endl;

	printMatrix(matrixGet, size);


	logFile.close();
	//
	//for (int i = 0; i < size; i++)
	//	delete[] matrixGenerated2D[i];
	//delete[] matrixGenerated;

	delete[] matrixGet;
	delete[] matrixGenerated;

	MPI_Finalize();
	return 0;
}

//void sendPackMatrix(double** matrix, int size, int rank) {
//
//	buf = new char[packSize];
//	int position = 0;
//
//	for (int i = 0; i < size; i++) {
//		for (int j = 0; j < size; j++) {
//			MPI_Pack(&i, 1, MPI_INT, buf, packSize, &position, MPI_COMM_WORLD);
//			MPI_Pack(&j, 1, MPI_INT, buf, packSize, &position, MPI_COMM_WORLD);
//			MPI_Pack(&matrix[i][j], 1, MPI_DOUBLE, buf, packSize, &position, MPI_COMM_WORLD);
//		}
//	}
//	MPI_Send(&packSize, 1, MPI_INT, rank, 321, MPI_COMM_WORLD);
//	MPI_Send(buf, packSize, MPI_PACKED, rank, 123, MPI_COMM_WORLD);
//	delete[] buf;
//}

//void recvPackMatrix(double** matrix, int size, int rank) {
//	MPI_Status status;
//	int position = 0, j, i;
//	double elements;
//
//	MPI_Recv(&packSize, 1, MPI_INT, rank, 321, MPI_COMM_WORLD, &status);
//	buf = new char[packSize];
//	MPI_Recv(buf, packSize, MPI_PACKED, rank, 123, MPI_COMM_WORLD, &status);
//
//	//for (int i = 0; i < size; i++)
//	//{
//	//	for (int j = 0; j < size; j++) {
//	//		MPI_Unpack(buf, packSize, &position, &elements, 1, MPI_DOUBLE, MPI_COMM_WORLD);
//	//		matrix[i][j] = elements;
//	//	}
//	//}
//
//	for (int k = 0; k < size; k++)
//	{
//		MPI_Unpack(buf, packSize, &position, &i, size, MPI_INT, MPI_COMM_WORLD);
//		MPI_Unpack(buf, packSize, &position, &j, size, MPI_INT, MPI_COMM_WORLD);
//		MPI_Unpack(buf, packSize, &position, &elements, size, MPI_DOUBLE, MPI_COMM_WORLD);
//		matrix[i][j] = elements;
//	}
//
//	cout << elements;
//	delete[] buf;
//}

void logFileFunction(int rank) {
	logFile.open("log.proc_" + to_string(rank), fstream::out | fstream::trunc);
}

//void printMatrix(double* matrix, int size) {
//	for (int i = 0; i < size*size; i++)
//	{
//		logFile << matrix[i] << " ";
//	}
//	logFile << endl;
//}

void printMatrix(double* matrix, int size) {
	for (int i = 0; i < size; i++)
	{
		for (int j = 0; j < size; j++)
		{
			logFile << matrix[i*size + j] << " ";
		}
		logFile << endl;
	}
}