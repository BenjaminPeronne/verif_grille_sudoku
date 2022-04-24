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

    for (i = 0; i < 9; i++) { // Check the row of the (x, y) position
        if (tabs[thread][x][i] == z) // If the element is already in the row
            return (FALS); // The element can't be placed in the (x, y) position
        if (tabs[thread][i][y] == z) // If the element is already in the column
            return (FALS); // The element can't be placed in the (x, y) position
    }
    // Check the square
    pi = (x / 3) * 3; // The first row of the square
    pj = y - y % 3;   // The first column of the square
    for (i = 0; i < 3; i++) // Check the square of the (x, y) position
        for (j = 0; j < 3; j++) // Check the square of the (x, y) position
            if (tabs[thread][pi + i][pj + j] == z) // If the element is already in the square
                return (FALS); // The element can't be placed in the (x, y) position
    return (CERT); // The element can be placed in the (x, y) position
} 

////////////////////////////////////////////////////////////////////
// The function is called when the element can be placed in the (x, y) position
int check(int i, int j, int thread) { // Check if the element can be placed in the (i, j) position
    int k;
    long int s = 0;
    // int thread = omp_get_thread_num();

    if (tabs[thread][i][j]) // Fixed value should not be iterated
    {
        if (j < 8) // Check the next column of the table
            return (check(i, j + 1, thread)); // Recursive call
        else if (i < 8)
            return (check(i + 1, 0, thread)); // Recursive call
        else
            return (1); // End of tab
    } else              // There is a 0 we have to try
    {
        for (k = 1; k < 10; k++) // Try all the numbers
            if (can_be_assigned(i, j, k, thread)) { // If it is possible to put k in (i, j)
                tabs[thread][i][j] = k; // Put k in (i, j)
                if (j < 8) // Check the next column of the table
                    s += check(i, j + 1, thread); // Recursive call to check the next column of the table (j+1)
                else if (i < 8) // Check the next row of the table
                    s += check(i + 1, 0, thread); // Recursive call (i+1, 0)
                else // End of tab
                    s++;  // We have found a solution
                tabs[thread][i][j] = 0; // Remove k from (i, j)
            }
    }
    return (s); // Return the number of solutions
}

////////////////////////////////////////////////////////////////////
// Check if the sudoku is solved (1 solution)
int is_solve(int i, int j) { // Check if the element can be placed in the (i, j) position 
    int k; // Counter for the loop on the numbers 
    long int s = 0; // Counter for the number of solutions
    int thread = 0; // The thread number 

    if (tabs[thread][i][j]) { // Fixed value should not be iterated
        if (j < 8) // Check the next column
            return (is_solve(i, j + 1)); // Go to the next column
        else if (i < 8) // Check the next row
            return (is_solve(i + 1, 0)); // Go to the next row
        else 
            return (1); // 
    } else {
#pragma omp parallel firstprivate(thread) 
        {
#pragma omp for reduction(+ \
                          : s)
            for (k = 1; k < 10; k++) { // There is a 0 we have to try
                clock_t start = clock();
                thread = omp_get_thread_num(); // Get the thread number 
                if (can_be_assigned(i, j, k, thread)) { // Check if the number can be assigned
                    tabs[thread][i][j] = k; // Assign the number
                    if (j < 8) // Check the next column
                        s += check(i, j + 1, thread); // Go to the next column 
                    else if (i < 8) // Check the next row
                        s += check(i + 1, 0, thread); // Go to the next row
                    else // End of tab
                        s++; // Increment the solution counter
                    tabs[thread][i][j] = 0; // Reset the value of the cell
                    printf("%d Results using thread %d: %li\n", k, thread, s);
                    clock_t end = clock();
                    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
                    printf("Time spent: %f\n", time_spent);
                }
            }
        }
    }
    return (s); // Return the number of solutions 
}

////////////////////////////////////////////////////////////////////
int main() {
    // int i,j,k;
    long int nsol;

    init_tab();
    // start clock
    // clock_t start = clock();
    nsol = is_solve(0, 0);
    printf("Number of solutions : %ld\n", nsol);
    // end clock
    // clock_t end = clock();
    // double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    // printf("Time spent: %f\n", time_spent);
    exit(0);
}
