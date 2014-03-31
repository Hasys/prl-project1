#include <mpi.h>
#include <stdio.h>

#define X_TAG 0
#define Y_TAG 1
#define Z_TAG 2
#define OUT_TAG 3

int isExistsInArray (unsigned char, unsigned char *, int);

int main (int argc, char *argv[]) {
    unsigned char XB, YB, ZB, OB;
    int numprocs, computers,
        myid,
        i;

    MPI_Status stat;
    
    MPI_Init(&argc, &argv); /* inicializace MPI */
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs); /* zjistíme, kolik procesů běží */
    MPI_Comm_rank(MPI_COMM_WORLD, &myid); /* zjistíme id svého procesu */

    computers = numprocs - 1;
    if (myid == 0) {
        unsigned char numbersTmp[computers],
            numbers[computers],
            numbersSorted[computers];

        // Načtění hodnod
        FILE *fp = fopen("numbers", "r");
        fread(numbersTmp, 1, computers, fp);
        fclose(fp);
        
        for (i = 0; i < computers; i += 1) {
            numbers[i] = -1;
        }

        // Odstranění duplikatů
        for (i = 0; i < computers; i += 1) {
            if (!isExistsInArray(numbersTmp[i], numbers, computers)) {
                numbers[i] = numbersTmp[i];
            } else {
                while(isExistsInArray(numbersTmp[i], numbers, computers)) {
                    numbersTmp[i] += 1;
                    if (numbersTmp[i] > 255) {
                        numbersTmp[i] = 0;
                    }
                }
                numbers[i] = numbersTmp[i];
            }
        }

        for (i = 0; i < computers; i += 1) {
            printf("%d ", numbers[i]);
        }
        printf("\n");

        // (krok 2.1 odesilání zprav)
        for (i = 0; i < computers; i += 1) {
            MPI_Send(&numbers[i], 1, MPI_INT, i + 1, X_TAG, MPI_COMM_WORLD);
            MPI_Send(&numbers[i], 1, MPI_INT, 1, Y_TAG, MPI_COMM_WORLD);
        }

        // (krok 3)
        for (i = 1; i <= computers; i += 1) {
            MPI_Recv(&OB, 1, MPI_INT, computers, OUT_TAG, MPI_COMM_WORLD, &stat);
            numbersSorted[computers - i] = OB;
        }

        for (i = 0; i < computers; i += 1) {
            printf("%d\n", numbersSorted[i]);
        }
    } else {
        // (krok 1)
        int C = 1, X = -1, Y = -1, Z = -1, 
            receiver;

        // (krok 2.1 posuv hodnot do registru a posuv všech registrů Y doprava)
        MPI_Recv(&XB, 1, MPI_INT, 0, X_TAG, MPI_COMM_WORLD, &stat);
        X = XB;
        // První má n hodnot od nultého procesoru
        for (i = 0; i < computers; i += 1) {
            MPI_Recv(&YB, 1, MPI_INT, myid - 1, Y_TAG, MPI_COMM_WORLD, &stat);
            Y = YB;
            // (krok 2.2)
            C = (X > Y) ? (C + 1) : C;
            if (myid < computers) {
                MPI_Send(&Y, 1, MPI_INT, myid + 1, Y_TAG, MPI_COMM_WORLD);
            }
        }

        // Přesuneme hodnoty z registru X do registru procesora s indexem C (krok 2.3)
        MPI_Send(&X, 1, MPI_INT, C, Z_TAG, MPI_COMM_WORLD);
        MPI_Recv(&ZB, 1, MPI_INT, MPI_ANY_SOURCE, Z_TAG, MPI_COMM_WORLD, &stat);
        Z = ZB;

        receiver = (myid == computers) ? 0 : myid + 1;
        
        // (krok 3)
        for (i = 1; i < myid; i += 1) {
            // Nevím jak to možně, ale na marlinovi (jen na merlinovi) ne funguje bez tenhle kroku
            receiver = (myid == computers) ? 0 : myid + 1;
            MPI_Send(&Z, 1, MPI_INT, receiver, OUT_TAG, MPI_COMM_WORLD);
            MPI_Recv(&ZB, 1, MPI_INT, myid - 1, OUT_TAG, MPI_COMM_WORLD, &stat);
            Z = ZB;
        }

        MPI_Send(&Z, 1, MPI_INT, receiver, OUT_TAG, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

int isExistsInArray (unsigned char x, unsigned char array[], int len) {
    int i;
    for (i = 0; i < len; i += 1) {
        if (array[i] == x) {
            return 1;
        }
    }
    return 0;
}