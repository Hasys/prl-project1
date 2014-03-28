#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define X_TAG 0
#define Y_TAG 1

int main(int argc, char *argv[]) {
    unsigned char XB, YB;
    int numprocs,
        myid,
        i,j;

    MPI_Status stat;
    
    MPI_Init(&argc, &argv); /* inicializace MPI */
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); /* zjistíme, kolik procesů běží */
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); /* zjistíme id svého procesu */

    if (myid == 0) {
        int numbersTmp[numprocs - 1],
            numbers[numprocs - 1];
            
        FILE *fp = fopen("numbers", "r");
        fread(numbersTmp, 1, numprocs - 1, fp);
        fclose(fp);
        
        for (i = 0; i < numprocs - 1; i += 1) {
            numbers[i] = -1;
        }

        for (i = 0; i < numprocs - 1; i += 1) {
            if (!isExistsInArray(numbersTmp[i], numbers)) {
                numbers[i] = numbersTmp[i];
            } else {
                while(isExistsInArray(numbersTmp[i], numbers)) {
                    numbersTmp[i] += 1;
                }
                numbers[i] = numbersTmp[i];
            }
        }

        printf("%d: We have %d processors\n", myid, numprocs);
        for (i = 0; i < numprocs - 1; i += 1) {
            MPI_Send(&numbers[i], 1, MPI_INT, i+1, X_TAG, MPI_COMM_WORLD);
        }

        MPI_Send(&numbers[0], 1, MPI_UNSIGNED_CHAR, 1, Y_TAG, MPI_COMM_WORLD);
    } else {
        int C = 1, X = -1, Y = -1, Z = -1;

        MPI_Recv(&XB, 1, MPI_INT, 0, X_TAG, MPI_COMM_WORLD, &stat);
        X = XB;
        printf("X_TAG! Processor %d. Registers values: C=%d, X=%d, Y=%d, Z=%d.\n", myid, C, X, Y, Z);

        if (myid == 1) {
            MPI_Recv(&YB, 1, MPI_INT, 0, Y_TAG, MPI_COMM_WORLD, &stat);
            Y = YB;
            printf("Y_TAG! Processor %d. Registers values: C=%d, X=%d, Y=%d, Z=%d.\n", myid, C, X, Y, Z);
        }
    }

    MPI_Finalize();
    return 0;
}

int isExistsInArray(int x, int array[]) {
    int len = sizeof(array) / sizeof(int),
        i;
    for (i = 0; i < len; i += 1) {
        if (array[i] == x) {
            return 1;
        }
    }
    return 0;
}