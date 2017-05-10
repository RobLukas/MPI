#include <iostream>
#include <mpi.h>
#include <ctime>
#include <fstream>
#include <string>
#include <random>
#include <time.h>
#include <cstdlib>

void logFileFunction(int);
void printMatrix(double*, int);

using namespace std;

fstream logFile;

int main(int argc, char* argv[]) {

	MPI::Init(argc, argv);

	int rank, size;
	//numer obecnego procesora i liczba procesorów
	rank = MPI::COMM_WORLD.Get_rank();
	size = MPI::COMM_WORLD.Get_size();

	//double **matrixGenerated2D = new double *[size];
	double *matrixGenerated = new double[size*size];
	double *matrixGet = new double [size*size];

	srand((unsigned)time(NULL) + rank*rank + size);

	//int k = 0;
	for (int i = 0; i < size; i++) {
		//matrixGenerated2D[i] = new double[size];
		for (int j = 0; j < size; j++) {
			double r = ((double)rand() / (RAND_MAX));
			matrixGenerated[size * i + j] = r;
			//matrixGenerated2D[i][j] = r;
			//matrixGenerated[k++] = matrixGenerated2D[i][j];
		}
	}

	logFileFunction(rank);

	if ((rank % 2) == 0) {
		MPI::COMM_WORLD.Send(matrixGenerated, size*size, MPI_DOUBLE, rank + 1, 123);
		MPI::COMM_WORLD.Recv(matrixGet, size*size, MPI_DOUBLE, rank + 1, 123);
	}
	else
	{
		MPI::COMM_WORLD.Recv(matrixGet, size*size, MPI_DOUBLE, rank - 1, 123);
		MPI::COMM_WORLD.Send(matrixGenerated, size*size, MPI_DOUBLE, rank - 1, 123);
	}


	if ((rank % 2) == 0)
		logFile << "matrixGenerated sent to process: " << rank + 1 << endl;
	else
		logFile << "matrixGenerated sent to process: " << rank - 1 << endl;
	printMatrix(matrixGenerated, size);

	if ((rank % 2) == 0)
		logFile << "Receive matrixGet from process: " << rank + 1 << endl;
	else
		logFile << "Receive matrixGet from process: " << rank - 1 << endl;
	printMatrix(matrixGet, size);
	cout << "Files created"<<endl;

	cout << "log.proc_" + to_string(rank) << endl;

	logFile.close();

	//for (int i = 0; i < size; i++)
	//	delete[] matrixGenerated2D[i];
	//delete[] matrixGenerated;

	delete[] matrixGet;
	delete[] matrixGenerated;

	MPI_Finalize();
	return 0;
}

void logFileFunction(int rank) {
	logFile.open("log.proc_" + to_string(rank), fstream::out);
}

void printMatrix(double* matrix, int size) {
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
		logFile << matrix[size * i + j] << " ";
	}
	logFile<<endl;
	}
}

//zakleszczenia nie wystepuje poniewaz najpierw nadaje wiadomosc a potem odbiera na tym samym procesorze, pozniej odbiera a potem nadaje na drugim procesorze

//void printMatrix(double* matrix, int size) {
//	for (int i = 0; i < size*size; i++)
//	{
//		logFile << matrix[i] << " ";
//	}
//	logFile << endl;
//}
