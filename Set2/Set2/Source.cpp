#include <mpi.h>
#include <iostream>
#include <time.h>
#include <fstream>
#include "Vector.h"

using namespace std;

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	Vector *vector1, *vector2, *vector3;
	vector1 = new Vector;
	vector2 = new Vector(2, 4, 1);
	vector3 = new Vector(1, 20, 4);

	string vectors = "vectors";
	ofstream vectorsText(vectors.c_str());

	vectorsText << "<" + vector1->getX + " " + vector1->getY + " " + vector1->getZ + ">" << endl;
	vectorsText << "<" + vector2->getX + " " + vector2->getY + " " + vector2->getZ + ">" << endl;
	vectorsText << "<" + vector3->getX + " " + vector3->getY + " " + vector3->getZ + ">" << endl;
	


	MPI_Finalize();
	return 0;
}