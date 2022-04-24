#include <assert.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CERT 1
#define FALS 0
#define DEPTH 6
#define TABSIZE 600

typedef struct { 
    int tabs[9][9]; // Sudoku struct definition 
    int i; // i is the index of the current thread
    int j; // j is the index of the current column
} sudoku;

sudoku *grids;
int pointer;

// Doit être correct initialement !!
int tabs[9][9] =
    {1, 2, 3, 4, 5, 6, 7, 8, 9,
     9, 8, 7, 3, 2, 1, 6, 5, 4,
     6, 5, 4, 7, 8, 9, 1, 2, 3,

     7, 9, 8, 1, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0,

     0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0};


void read_file(char * filename) {
    FILE * fp;
    int i, j;
    fp = fopen(filename, "r");
    assert(fp != NULL);
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            fscanf(fp, "%d", &tabs[i][j]);
        }
    }
    fclose(fp);
}

void copy_table(int new_table[][9], int last_table[][9]) {
    int i, j;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            new_table[i][j] = last_table[i][j];
        }
    }
}

int can_be_assigned(int x, int y, int z, int tabs[][9]) //
{
    int i, j, pi, pj;

    for (i = 0; i < 9; i++)
        if (tabs[x][i] == z)
            return (FALS); // Rangés
    for (i = 0; i < 9; i++)
        if (tabs[i][y] == z)
            return (FALS); // Columnes
    // Quadrat
    pi = (x / 3) * 3; // Primer x
    pj = y - y % 3;   // Primer y
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            if (tabs[pi + i][pj + j] == z)
                return (FALS);
    return (CERT);
}

////////////////////////////////////////////////////////////////////
// Fonction qui va tester si le sudoku est correct
int check(int i, int j, int tabs[][9]) //
{
    int k;
    long int s = 0;

    if (tabs[i][j]) // La valeur fixe ne doit pas être itérée
    {
        if (j < 8)
            return (check(i, j + 1, tabs));
        else if (i < 8)
            return (check(i + 1, 0, tabs));
        else
            return (1); // Fin des tabs
    } else              // La valeur est 0
    {
        for (k = 1; k < 10; k++)
            if (can_be_assigned(i, j, k, tabs)) {
                tabs[i][j] = k;
                if (j < 8)
                    s += check(i, j + 1, tabs);
                else if (i < 8)
                    s += check(i + 1, 0, tabs);
                else
                    s++;
                tabs[i][j] = 0;
            }
    }
    return (s);
}

void init_grids(int i, int j, int level) //
{
    int k;

    if (level > DEPTH) { // Si nous avons déjà atteint la profondeur maximale, nous ne faisons rien
        // new elem to list
        copy_table(grids[pointer].tabs, tabs);
        grids[pointer].i = i;
        grids[pointer].j = j;
        pointer++;
    } else if (tabs[i][j]) // La valeur fixe ne doit pas être itérée
    {
        if (j < 8)
            return (init_grids(i, j + 1, level));
        else if (i < 8)
            return (init_grids(i + 1, 0, level));
    } else // il y a un 0 que nous devons essayer
    {
        for (k = 1; k < 10; k++)
            if (can_be_assigned(i, j, k, tabs)) {
                tabs[i][j] = k;
                if (j < 8)
                    init_grids(i, j + 1, level + 1);
                else if (i < 8)
                    init_grids(i + 1, 0, level + 1);
                else {
                    copy_table(grids[pointer].tabs, tabs);
                    grids[pointer].i = i;
                    grids[pointer].j = j;
                    pointer++;
                }
                tabs[i][j] = 0;
            }
    }
}
// Print the table
void print_table(int tabs[][9]) {
    int i, j;
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            printf("%d ", tabs[i][j]);
        }
        printf("\n");
    }
}

void addem(long int *invec, long int *inoutvec, int *len, MPI_Datatype *dtype) { //
    int i;
    for (i = 0; i < *len; i++)
        inoutvec[i] += invec[i];
}

////////////////////////////////////////////////////////////////////
int main(int nargs, char *args[]) {
    long int nsol = 0;
    long int sol;
    
    int id, n_procc;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procc);

    grids = (sudoku *)malloc(sizeof(sudoku) * TABSIZE);
    pointer = 0;
    init_grids(0, 0, 0);

    int num_elements_per_proc = pointer / n_procc;
    int id_limit = pointer % n_procc;
    int i = 0;
    int j = 0;
    while (i < id) {
        if (i < id_limit)
            j += num_elements_per_proc + 1;
        else
            j += num_elements_per_proc;
        i++;
    }

    // Start timer MPI
    double start_time = MPI_Wtime();

    if (id < id_limit)
        pointer = j + num_elements_per_proc + 1;
    else
        pointer = j + num_elements_per_proc;
    for (; j < pointer; j++) {
        nsol += check(grids[j].i, grids[j].j, grids[j].tabs);
    }

    printf("node %d found %ld solutions\n", id, nsol);

    MPI_Op op;
    MPI_Op_create((MPI_User_function *)addem, 1, &op);
    MPI_Reduce(&nsol, &sol, 1, MPI_LONG_INT, op, 0, MPI_COMM_WORLD);
    // MPI_Reduce( &nsol, &sol, 1, MPI_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD );

    if (id == 0) {
        printf("For tab Size %d \n", TABSIZE);
        printf("Number of solutions : %ld\n", sol);
        print_table(grids[0].tabs);
    }

    // End timer MPI
    double end_time = MPI_Wtime();
    double total_time = end_time - start_time;
    printf("node %d total time: %f\n", id, total_time);

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    exit(0);
}
