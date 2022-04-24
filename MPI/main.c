/**
 * @author Benjamin Peronne
 * @email contact@benjaminperonne.fr
 * @create date 2022-04-22 15:22:47
 * @modify date 2022-04-22 15:22:47
 * @desc [MPI]
 */

#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9
#define UNASSIGNED 0
#define CERT 1
#define FALS 0
#define DEPTH 6

// #include "../header.h"

typedef struct sudoku { // Sudoku struct definition
    int **grid;
    int size;
} sudoku;

sudoku * grids;

void *protected_malloc(size_t size) {
    void *ptr = NULL;
    if ((ptr = malloc(size)) == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}
// Initialize the sudoku grid
void sudoku_init(sudoku *s, int size) {
    s->size = size;
    s->grid = protected_malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++) {
        s->grid[i] = protected_malloc(sizeof(int) * size);
    }
}

// Initialize the sudoku grid from a file
void sudoku_init_from_file(sudoku *s, char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: file %s not found\n", file_name);
        exit(EXIT_FAILURE);
    }
    sudoku_init(s, SIZE);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            fscanf(file, "%d", &s->grid[i][j]);
        }
    }
    fclose(file);
}

void sudoku_free(sudoku *s) { // Free the sudoku grid memory
    for (int i = 0; i < s->size; i++) {
        free(s->grid[i]);
    }
    free(s->grid);
}

void sudoku_print(sudoku *s) { // Print the sudoku grid
    printf("-------------------------\n");
    for (int i = 0; i < s->size; i++) {
        printf("| ");
        for (int j = 0; j < s->size; j++) {
            printf("%d ", s->grid[i][j]);
            if ((j + 1) % 3 == 0) {
                printf("| ");
            }
        }
        printf("\n");
        if (i == 2 || i == 5) {
            printf("-------------------------\n");
        }
    }
    printf("-------------------------\n");
}

int is_exist_row(sudoku *s, int row, int value) { // Check if the value is already in the row
    for (int i = 0; i < s->size; i++) {
        if (s->grid[row][i] == value) {
            return 1;
        }
    }
    return 0;
}

int is_exist_col(sudoku *s, int col, int value) { // Check if the value is already in the column
    for (int i = 0; i < s->size; i++) {
        if (s->grid[i][col] == value) {
            return 1;
        }
    }
    return 0;
}

int is_exist_square(sudoku *s, int row, int col, int value) { // Check if the value is already in the square
    int square_size = sqrt(s->size);
    int square_row = row - (row % square_size);
    int square_col = col - (col % square_size);
    for (int i = square_row; i < square_row + square_size; i++) {
        for (int j = square_col; j < square_col + square_size; j++) {
            if (s->grid[i][j] == value) {
                return 1;
            }
        }
    }
    return 0;
}

int is_safe_number(sudoku *s, int row, int col, int value) { // Check if the value is safe to put in the grid
    if (is_exist_row(s, row, value) || is_exist_col(s, col, value) || is_exist_square(s, row, col, value)) {
        return 0; // The value is already in the grid
    }
    return 1; // The value is safe to put in the grid
}

int find_unassigned_location(sudoku *s, int *row, int *col) { // Find the first unassigned location
    for (int i = 0; i < s->size; i++) {
        for (int j = 0; j < s->size; j++) {
            if (s->grid[i][j] == UNASSIGNED) {
                *row = i;
                *col = j;
                return 1;
            }
        }
    }
    return 0;
}

// New version of solve_sudoku
int solve_sudoku_rec(sudoku *s, int row, int col) { // Solve the sudoku recursively
    int square_size = sqrt(s->size);
    if (row == s->size) { // If the row is the last one
        return 1;
    }
    if (s->grid[row][col] != UNASSIGNED) { // If the location is already assigned, go to the next one
        if (col == s->size - 1) {
            return solve_sudoku_rec(s, row + 1, 0); // Go to the next row
        } else {
            return solve_sudoku_rec(s, row, col + 1); // Go to the next column
        }
    }
    for (int i = 1; i <= s->size; i++) {      // Try all the possible values
        if (is_safe_number(s, row, col, i)) { // If the value is safe, assign it to the grid
            s->grid[row][col] = i;
            if (col == s->size - 1) {                  // If the column is the last one, go to the next row
                if (solve_sudoku_rec(s, row + 1, 0)) { // If the sudoku is solved, return 1
                    return 1;                          // Return 1 if the sudoku is solved
                }
            } else {
                if (solve_sudoku_rec(s, row, col + 1)) {
                    return 1;
                }
            }
            s->grid[row][col] = UNASSIGNED;
        }
    }
    return 0;
}

int solve_sudoku(sudoku *s) { // Solve the sudoku
    int row = 0;
    int col = 0;
    if (!find_unassigned_location(s, &row, &col)) {
        return 1;
    }
    return solve_sudoku_rec(s, row, col);
}

void sum_solutions(long int *invec, long int *inoutvec, int *len, MPI_Datatype *dptype) {
    int i;

    for (i = 0; i < *len; i++) {
        inoutvec[i] += invec[i];
    }
}

// Make 300 copies of the sudoku and solve them in parallel


int main(int argc, char *argv[]) {

    long int numberSol = 0; // Number of solutions
    long int sol;           // Solution

    int id, n_procc;
    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &n_procc);

    sudoku s;
    // make 300 sudoku grids
    grids = (sudoku *)malloc(sizeof(sudoku) * 500);

    sudoku_init_from_file(&s, "../9x9.txt");

    int num_element_per_proc = s.size / n_procc;
    int id_limit = s.size % num_element_per_proc;
    int i = 0;
    int j = 0;

    while (i < id) {
        if (i < id_limit)
            j += num_element_per_proc + 1;
        else
            j += num_element_per_proc;
        i++;
    }

    if (id < id_limit)
        s.size = j + num_element_per_proc + 1;
    else
        s.size = j + num_element_per_proc;
    for (; i < s.size; i++) {
        numberSol += solve_sudoku(grids);
    }

    printf("node %d found %ld solutions\n", id, numberSol);

    MPI_Op op;
    MPI_Op_create((MPI_User_function *)sum_solutions, 1, &op);
    MPI_Reduce(&numberSol, &sol, 1, MPI_LONG, op, 0, MPI_COMM_WORLD);

    if (id == 0) {
        printf("The number of solutions is %ld\n", sol);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    sudoku_free(&s);
    sudoku_free(grids);
    exit(0);
}
