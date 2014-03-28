#include <mpi.h>
#include <stdio.h>

#define X_TAG 0
#define Y_TAG 1
#define COUNT_TAG 2

int main(int argc, char *argv[]) {
    unsigned char XB;
    unsigned char YB;
    unsigned char buffer;
    int numprocs,
        myid,
        i,j;

    MPI_Status stat;
    
    MPI_Init(&argc, &argv); /* inicializace MPI */
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); /* zjistíme, kolik procesů běží */
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); /* zjistíme id svého procesu */

    numprocs -= 1;
    if (myid == 0) {
        char numbersTmp[numprocs],
            numbers[numprocs];

        FILE *fp = fopen("numbers", "r");
        fread(numbersTmp, 1, numprocs, fp);
        fclose(fp);
        
        for (i = 0; i < numprocs; i += 1) {
            numbers[i] = -1;
        }

        for (i = 0; i < numprocs; i += 1) {
            if (!isExistsInArray(numbersTmp[i], numbers)) {
                numbers[i] = numbersTmp[i];
            } else {
                while(isExistsInArray(numbersTmp[i], numbers)) {
                    numbersTmp[i] += 1;
                }
                numbers[i] = numbersTmp[i];
            }
        }

        printf("%d: We have %d processors\n", myid, (numprocs + 1));
        for (i = 0; i < numprocs; i += 1) {
            MPI_Send(&numprocs, 1, MPI_INT, i + 1, COUNT_TAG, MPI_COMM_WORLD);
        }

        for (i = 0; i < 2 * numprocs; i += 1) {
            if (i < numprocs) {
                MPI_Send(&numbers[i], 1, MPI_INT, i + 1, X_TAG, MPI_COMM_WORLD);
                MPI_Send(&numbers[i], 1, MPI_INT, 1, Y_TAG, MPI_COMM_WORLD);
            } else {

            }
        }

    } else {
        int C = 1, X = -1, Y = -1, Z = -1, 
            count = -1;

        MPI_Recv(&buffer, 1, MPI_INT, 0, COUNT_TAG, MPI_COMM_WORLD, &stat);
        count = buffer;

        MPI_Recv(&XB, 1, MPI_INT, 0, X_TAG, MPI_COMM_WORLD, &stat);
        X = XB;

        for (i = 0; i < count; i += 1) {
            MPI_Recv(&YB, 1, MPI_INT, myid - 1, Y_TAG, MPI_COMM_WORLD, &stat);
            Y = YB;
            C = (X > Y) ? (C + 1) : C;
            if (myid < count) {
                MPI_Send(&Y, 1, MPI_INT, myid + 1, Y_TAG, MPI_COMM_WORLD);
            }
        }

        printf("Processor %d. Registers values: C=%d, X=%d, Y=%d, Z=%d.\n", myid, C, X, Y, Z);
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