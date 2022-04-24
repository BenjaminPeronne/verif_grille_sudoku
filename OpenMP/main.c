/**
 * @author Benjamin Peronne
 * @email contact@benjaminperonne.fr
 * @create date 2022-04-22 15:39:26
 * @modify date 2022-04-22 15:39:26
 * @desc [OpenMp]
 */


#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define SIZE 9
#define UNASSIGNED 0

#include "../header.h"


// Main function to solve the sudoku with OpenMP parallelization
int main(int argc, char *argv[]) {
    sudoku s;
    sudoku_init_from_file(&s, argv[1]);
    sudoku_print(&s);
    clock_t start = clock();
    if (solve_sudoku(&s)) {
        sudoku_print(&s);
    } else {
        printf("No solution found\n");
    }
    clock_t end = clock();
    printf("Time taken: %f seconds\n", (double)(end - start) / CLOCKS_PER_SEC);
    sudoku_free(&s);
    return 0;
}