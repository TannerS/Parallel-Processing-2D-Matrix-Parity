

#include "mpi.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>


#define MaxRow 20000
#define MaxColumn 20000

// argc = cpu count, argv = file.cpp
int main(int argc, char *argv[])
{
	srand(time(0));
	int BV[MaxRow][MaxColumn];
	// parity array
	int parity[MaxRow];
	//BV of size MaxRow x MaxColum
	int myid, numprocs = 0;
	// processor name
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int count = 0;
	int namelen = 0;
	int paritys[MaxRow];
	int new_paritys[MaxRow];
	//initialize MPI execution environment
	MPI_Init(&argc, &argv);
	//each process get total # of processes 
	//the total # of processes specified in mpirun –np n
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	//each process gets its own id
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	// Gets the name of the processor
	MPI_Get_processor_name(processor_name, &namelen);
	// display info
	fprintf(stderr, "process %d on %s\n", myid, processor_name);
	fflush(stderr);
	// broken up array size (rows split up with same cols)
	int rows = (int)ceil((double)MaxRow / numprocs);
	// returned fraction of the array (after scatter)
	// kept it same size due to limitations in C where
	// int has to be a constent value
	// so it will be this size but we will only use the bounds 
	// up until rows and MaxColumn
	int RV[MaxRow][MaxColumn];
	// master fills up the matrix
	printf("filling up array\n");
	if (myid == 0)
	{
		// Fill up array with random 0's and 1's
		for (int i = 0; i < MaxRow; i++)
		{
			for (int j = 0; j < MaxColumn; j++)
				BV[i][j] = rand() % 2;
		}

		//display data for debugging
		//printf("ROOT Processor %d has data:\n", myid);
		/*
		for (int i = 0; i < MaxRow; i++)
		{
			for (int j = 0; j < MaxColumn; j++)
			{
				//printf("%d ", BV[i][j]);
			}

			//printf("\n");
		}*/
	}

		// int MPI_Scatter(const void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm comm)
		// we will sue scatter to break up the array to other processes
		// with rows being the divided about of rows, and columns size, this is what we pass in
		// RV is the allocated array rows*cols that we be sent to use after they are broken up
		//MPI_Scatter(BV, (rows*MaxColumn), MPI_INT, RV, (rows*MaxColumn), MPI_INT, 0, MPI_COMM_WORLD);
		// the sizes have to match, so we use rows instead of MaxRows
		MPI_Scatter(BV, (rows*MaxColumn), MPI_INT, RV, (rows*MaxColumn), MPI_INT, 0, MPI_COMM_WORLD);
		//printf("Processor %d has data: \n", myid);
		// we send and recieve the total size but we only need to do work on 
		// up until rows, not MaxRows
		// for debugging
		/*
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < MaxColumn; j++)
			{
				//printf("%d ", RV[i][j]);
			}

			//printf("\n");
		}*/

		//get parity count, this happens for all processes
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < MaxColumn; j++)
			{
				if (RV[i][j] == 1)
					count++;
			}
			// even ammounts of 1's, parity is 1
			if (count % 2 == 0)
			{
				parity[i] = 1;
				//printf("Row: %d has parity of %d\n", i, 1);
			}
			else
			{
				parity[i] = 0;
				//printf("Row: %d has parity of %d\n", i, 0);
			}

			count = 0;
		}

		//MPI_Gather(sendarray, 100, MPI_INT, rbuf, 100, MPI_INT, root, comm);//
		MPI_Gather(parity, rows, MPI_INT, parity, rows, MPI_INT, 0, MPI_COMM_WORLD);
		
		/*
		if (myid == 0)
		{
			//printf("ROOT: Displaying parity now");

			for (int j = 0; j < MaxRow; j++)
			{
				//printf("Parity bit at row %d is %d\n", j, parity[j]);
			}

		}*/

		MPI_Finalize();
}
