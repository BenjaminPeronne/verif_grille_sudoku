#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CERT 1
#define FALS 0

int ***tabs;
int max_num_treads;

// Initialize the 3D array
int tab[9][9] =
    {1, 2, 3, 4, 5, 6, 7, 8, 9,
     9, 8, 7, 3, 2, 1, 6, 5, 4,
     6, 5, 4, 7, 8, 9, 1, 2, 3,

     7, 9, 8, 1, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0,

     0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0};

void init_tab() {
    max_num_treads = omp_get_max_threads();
    tabs = (int ***)malloc(sizeof(int **) * max_num_treads);
    for (int i = 0; i < max_num_treads; i++) {
        tabs[i] = (int **)calloc(9, sizeof(int *));
        for (int j = 0; j < 9; j++) {
            tabs[i][j] = (int *)calloc(9, sizeof(int));
            for (int k = 0; k < 9; k++) {
                tabs[i][j][k] = tab[j][k];
            }
        }
    }
}

void print_table(int table[][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("\t%d", table[i][j]);
        }
        printf("\n");
    }
}

// Check that the z element can be placed in the (x, y) position of sudoku
int can_be_assigned(int x, int y, int z, int thread) {
    int i, j, pi, pj;
    // int thread = omp_get_thread_num();

    for (i = 0; i < 9; i++) {
        if (tabs[thread][x][i] == z)
            return (FALS);
        if (tabs[thread][i][y] == z)
            return (FALS);
    }
    // Check the square
    pi = (x / 3) * 3; // Primer element de la fila
    pj = y - y % 3;   // Primer element de la columna
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            if (tabs[thread][pi + i][pj + j] == z)
                return (FALS);
    return (CERT);
}

////////////////////////////////////////////////////////////////////
int check(int i, int j, int thread) {
    int k;
    long int s = 0;
    // int thread = omp_get_thread_num();

    if (tabs[thread][i][j]) // Fixed value should not be iterated
    {
        if (j < 8)
            return (check(i, j + 1, thread));
        else if (i < 8)
            return (check(i + 1, 0, thread));
        else
            return (1); // End of tab
    } else              // There is a 0 we have to try
    {
        for (k = 1; k < 10; k++)
            if (can_be_assigned(i, j, k, thread)) {
                tabs[thread][i][j] = k;
                if (j < 8)
                    s += check(i, j + 1, thread);
                else if (i < 8)
                    s += check(i + 1, 0, thread);
                else
                    s++;
                tabs[thread][i][j] = 0;
            }
    }
    return (s);
}

////////////////////////////////////////////////////////////////////
int firstcheck(int i, int j) {
    int k;
    long int s = 0;
    int thread = 0;

    if (tabs[thread][i][j]) {
        if (j < 8)
            return (firstcheck(i, j + 1));
        else if (i < 8)
            return (firstcheck(i + 1, 0));
        else
            return (1); //
    } else {
#pragma omp parallel firstprivate(thread)
        {
#pragma omp for reduction(+ \
                          : s)
            for (k = 1; k < 10; k++) {
                clock_t start = clock();
                thread = omp_get_thread_num();
                if (can_be_assigned(i, j, k, thread)) {
                    tabs[thread][i][j] = k;
                    if (j < 8)
                        s += check(i, j + 1, thread);
                    else if (i < 8)
                        s += check(i + 1, 0, thread);
                    else
                        s++;
                    tabs[thread][i][j] = 0;
                    printf("%d Results using thread %d: %li\n", k, thread, s);
                    clock_t end = clock();
                    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
                    printf("Time spent: %f\n", time_spent);
                }
            }
        }
    }
    return (s);
}

////////////////////////////////////////////////////////////////////
int main() {
    // int i,j,k;
    long int nsol;

    init_tab();
    // start clock
    // clock_t start = clock();
    nsol = firstcheck(0, 0);
    printf("Number of solutions : %ld\n", nsol);
    // end clock
    // clock_t end = clock();
    // double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    // printf("Time spent: %f\n", time_spent);
    exit(0);
}
