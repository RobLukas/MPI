#include <mpi.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include "Vector.h"
#include <string>

using namespace std;

fstream vectorsText;

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	int textArray[3];
	char vector12[3];
	Vector *vector1, *vector2, *vector3;
	vector1 = new Vector;
	vector2 = new Vector(2.23, 4.12, 1.243);
	vector3 = new Vector(1, 20, 4);

	string vectors = "vectors";
	
	vectorsText.open("vectors", fstream::out | fstream::trunc);
	vectorsText << vector2->getX() << " " << vector2->getY() << " " << vector2->getZ() << endl;

	/*getline(vectorsText, vector12);*/
	vectorsText.read(vector12, 255);
	cout << vectorsText.gcount() << endl;
	cout << vector12;
	vectorsText.close();

	MPI_Finalize();
	return 0;
}