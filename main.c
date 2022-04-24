/**
 * @author Benjamin Peronne
 * @email contact@benjaminperonne.fr
 * @create date 2022-03-04 17:27:29
 * @modify date 2022-03-04 17:27:29
 * @desc [sudoku solver]
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 9
#define UNASSIGNED 0

#include "header.h"

int main(int argc, char const *argv[]) {
    sudoku s;
    sudoku_init_from_file(&s, "9x9.txt");
    // Print size
    printf("Size: %d\n", s.size);
    printf("Start solving...\n");
    // Print sudoku
    sudoku_print(&s);
    
    // Start the timer
    clock_t start = clock();
    // Solve the sudoku
    solve_sudoku(&s);
    // Stop the timer
    clock_t end = clock();
    // Print the sudoku
    sudoku_print(&s);
    // Print the time
    printf("Solve in :  %f s\n", (double)(end - start) / CLOCKS_PER_SEC);
    // Free the memory
    sudoku_free(&s);

    return 0;
}