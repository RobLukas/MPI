#include <iostream>
#include "mpi.h"
#include <fstream>

using namespace std;

int main(int argc, char** argv){

MPI::Init(argc, argv);

int rank = MPI::COMM_WORLD.Get_rank();
int size = MPI::COMM_WORLD.Get_size();

cout << "Hello World! I'm process " << rank << " and there are " << size << " processes in total!" << endl;


MPI::Finalize();

return 0;
}
