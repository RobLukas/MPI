#include <iostream>
#include <cstdlib>
#include "mpi.h"
#include <time.h>

using namespace std;

int main(int argc, char* argv[]){

MPI::Init(argc, argv);

int rank = MPI::COMM_WORLD.Get_rank();
int size = MPI::COMM_WORLD.Get_size();

if(rank == 0){
	int sum = 0;
	for(int i = 0; i < size - 1; i++){
		int integer = 0;
		MPI::Status status;
		MPI::COMM_WORLD.Recv(&integer, 1,  MPI::INT, MPI::ANY_SOURCE, MPI::ANY_TAG, status);
		//odbieram int i sprawdzam od jakiego procesora
		cout << "Received integer from process " << status.Get_source() << ": " << integer << endl;
		sum += integer;
	}
	//wysylam sume wszystkim procesom z mastera
	for(int i = 1; i < size; i++){
		MPI::COMM_WORLD.Send(&sum, 1, MPI::INT, i, i + size + 1);
	}
}

else {
	srand(rank + time(NULL));
	//losuje liczbe od 1 do n procesorow
	int r = rand() % size + 1;
	//wysylam losowe liczby o tagu aby odbiorca mogl zidetyfikowac wiadomosc
	MPI::COMM_WORLD.Send(&r, 1, MPI::INT, 0, 123);
	int sum = 0;
	MPI::Status status;
	//odbieranie sumy przez kolejne procesy od mastera
	MPI::COMM_WORLD.Recv(&sum, 1, MPI::INT, 0, MPI::ANY_TAG, status);
	cout << "Process " << rank << ": Received sum from master: " << sum << endl;
}

MPI::Finalize();
return 0;
}

//nie wymaga sie synchronizacji poniewaz kazdy procesor dysponuje wlasna pamiecia oraz nie jest to komunikacja zbiorowa
