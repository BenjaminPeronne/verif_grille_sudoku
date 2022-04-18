/**
 * @author Benjamin Peronne
 * @email contact@benjaminperonne.fr
 * @create date 2022-03-04 17:27:29
 * @modify date 2022-03-04 17:27:29
 * @desc [sudoku solver]
 */

//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include ./header.h

void sudoku_init(sudoku *s, int size); // Initialize a sudoku
void sudoku_free(sudoku *s);           // free the grid
void sudoku_print(sudoku *s);          // print the grid

int main(int argc, char const *argv[]) {
    int rank, size;

    MPI_Init(&argc, &argv); // Initialize the MPI environment
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get the rank of the current process
    MPI_Comm_size(MPI_COMM_WORLD, &size); // get the number of processes




    return 0;
}
