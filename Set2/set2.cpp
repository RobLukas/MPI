#include <iostream>
#include <mpi.h>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <iomanip> //setprecision
#include <iostream>
#include <fstream>
#include <string.h>
#include <sstream>
#include <cmath>

using namespace std;

void getLength(double*, double**, int);

int main(int argc, char** argv) {
	MPI::Init(argc, argv);

	fstream fileVec, fileTime;
	int lineByLine = 0;
	double startTime, endTime, time[4];

	int rank = MPI::COMM_WORLD.Get_rank();
	int size = MPI::COMM_WORLD.Get_size();

	startTime = MPI::Wtime();

	int vectorsForProcess, firstVector, lastVector, amountVectors;
	double * totalTime = new double[size * 4];

	ifstream openFile("v01");
	istream_iterator<double> start(openFile), end;
	vector<double> numbers(start, end);

	double averageSumLength[4];
	double sumOfVectors[4];

	amountVectors = numbers.size() / 3;
	int division = amountVectors / size;
	int rest = amountVectors % size;

	
	double ** vectors = new double *[amountVectors];
	for (int i = 0; i < amountVectors; i++)
	{
		vectors[i] = new double[3];
	}

	for (vector<double>::iterator it = numbers.begin(); it != numbers.end(); it++)
	{
		if (lineByLine == 3)
		{
			fileVec << endl;
			lineByLine = 0;
		}
		fileVec << *it << " ";
		lineByLine++;
	}

	vector<double>::iterator item = numbers.begin();
	for (int line = 0; line < amountVectors; line++)
	{
		for (int compositions = 0; compositions < 3; compositions++)
		{
			vectors[line][compositions] = *item;
			item++;
		}
	}

	if (rank < rest) {
		vectorsForProcess = division + 1;
		firstVector = rank*(division + 1);
	}
	else {
		vectorsForProcess = division;
		firstVector = rest*(division + 1) + (rank - rest) * division;
	}
	lastVector = firstVector + vectorsForProcess - 1;

	double ** vectorsTemp = new double *[lastVector - firstVector + 1];
	for (int i = 0; i <= lastVector - firstVector; i++)
	{
		vectorsTemp[i] = new double[3];
	}

	for (int lineNumber = firstVector, num = 0; lineNumber < lastVector, num < lastVector - firstVector; lineNumber++, num++)
	{
		for (int j = 0; j < 3; j++)
		{
			vectorsTemp[num][j] = vectors[lineNumber][j];
		}
	}

	fileVec.open("vec_" + to_string(rank), fstream::out | fstream::trunc);
	for (int lineNumber = firstVector; lineNumber < lastVector; lineNumber++) {
			for (int j = 0; j < 3; j++) {
				fileVec << vectors[lineNumber][j];
				fileVec << " ";
			}
			fileVec << endl;
	}
	fileVec.close();

	time[0] = MPI::Wtime() - startTime;

	getLength(sumOfVectors, vectorsTemp, lastVector - firstVector);

	time[1] = MPI::Wtime() - (startTime + time[0]);

	MPI_Reduce(&sumOfVectors, &averageSumLength, 4, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		for (int i = 0; i <= 3; i++)
		{
			averageSumLength[i] /= amountVectors;
		}
	}

	time[2] = MPI::Wtime() - (time[1] + startTime);
	endTime = MPI::Wtime();
	time[3] = endTime - startTime;
	

	MPI_Gather(time, 4, MPI_DOUBLE, totalTime, 4, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		double readData, processData, reduceResults, total;

		for (int i = 0; i < size; i++)
		{
			readData += totalTime[i * 4];
			processData += totalTime[i * 4 + 1];
			reduceResults += totalTime[i * 4 + 2];
			total += totalTime[i * 4 + 3];
		}
		fileTime.open("timer", fstream::out | fstream::trunc);

		for (int i = 0; i < size; i++)
		{
			fileTime << "timing (proc " << i << "):\n\t" << "readData:       " << totalTime[i * 4] << "\n\t" << "processData:    " << totalTime[i * 4 + 1] << "\n\t" << "reduceResults:  " << totalTime[i * 4 + 2] << "\n\t" << "total:          " << totalTime[i * 4 + 3] << endl;
			fileTime << endl;
		}
		fileTime << "total timings:\n\t\t" << "readData:       " << readData << "\n\t\t" << "processData:    " << processData << "\n\t\t" << "reduceResults:  " << reduceResults << "\n\t\t" << "total:          " << total << endl;
		fileTime << endl;
	}

	if (rank == 0)
	{
		cout << endl << "Average length: " << averageSumLength[3] << "\nAverage vector:\t[" << averageSumLength[0] << ", " << averageSumLength[1] << ", " << averageSumLength[2] << "]" << endl;
	}
	

	numbers.clear();
	for (int i = 0; i < amountVectors; i++)
	{
		delete[] vectors[i];
	}
	delete[] vectors;

	MPI::Finalize();
	return 0;
}

void getLength(double *SumArray, double **vectorsArray, int amount) {
	for (int i = 0; i < amount; i++)
	{
		SumArray[3] += sqrt((pow(vectorsArray[i][0], 2)) + (pow(vectorsArray[i][1], 2)) + (pow(vectorsArray[i][2], 2)));
		for (int j = 0; j < 3; j++) {
			SumArray[j] += vectorsArray[i][j];
		}
	}
}