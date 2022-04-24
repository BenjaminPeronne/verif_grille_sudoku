/**
 * @author Benjamin Peronne
 * @email contact@benjaminperonne.fr
 * @create date 2022-03-04 17:45:00
 * @modify date 2022-03-04 17:45:00
 * @desc [Sudoku function]
 */

typedef struct sudoku { // Sudoku struct definition with double pointer to int (grid) 
    int **grid; // Grid of the sudoku 
    int size; // Size of the sudoku 
} sudoku;

void *protected_malloc(size_t size) {
    void *ptr = NULL;
    if ((ptr = malloc(size)) == NULL) {
        fprintf(stderr, "Error: malloc failed\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void copy_sudoku(sudoku *s, sudoku *s_copy) {
    s_copy->size = s->size;
    s_copy->grid = protected_malloc(sizeof(int *) * s->size);
    for (int i = 0; i < s->size; i++) {
        s_copy->grid[i] = protected_malloc(sizeof(int) * s->size);
    }
    for (int i = 0; i < s->size; i++) {
        for (int j = 0; j < s->size; j++) {
            s_copy->grid[i][j] = s->grid[i][j];
        }
    }
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

// int solve_sudoku(sudoku *s) { // Solve the sudoku
//     int row, col;
//     if (!find_unassigned_location(s, &row, &col)) {
//         return 1;
//     }
//     for (int i = 1; i <= s->size; i++) {
//         if (is_safe_number(s, row, col, i)) {
//             s->grid[row][col] = i;
//             if (solve_sudoku(s)) {
//                 return 1;
//             }
//             s->grid[row][col] = UNASSIGNED;
//         }
//     }
//     return 0;
// }

// ==========================

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
    for (int i = 1; i <= s->size; i++) { // Try all the possible values
        if (is_safe_number(s, row, col, i)) { // If the value is safe, assign it to the grid
            s->grid[row][col] = i;
            if (col == s->size - 1) { // If the column is the last one, go to the next row
                if (solve_sudoku_rec(s, row + 1, 0)) { // If the sudoku is solved, return 1
                    return 1; // Return 1 if the sudoku is solved
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

int solve_sudoku_mpi(sudoku *s, int rank, int size) { // Solve the sudoku with MPI
    int row = 0;
    int col = 0;
    if (!find_unassigned_location(s, &row, &col)) {
        return 1;
    }
    int square_size = sqrt(s->size);
    int square_row = row - (row % square_size);
    int square_col = col - (col % square_size);
    int square_size_per_process = square_size / size;
    int square_row_start = square_row + rank * square_size_per_process;
    int square_row_end = square_row_start + square_size_per_process;
    int square_col_start = square_col;
    int square_col_end = square_col + square_size;
    for (int i = square_row_start; i < square_row_end; i++) {
        for (int j = square_col_start; j < square_col_end; j++) {
            if (s->grid[i][j] == UNASSIGNED) {
                for (int k = 1; k <= s->size; k++) {
                    if (is_safe_number(s, i, j, k)) {
                        s->grid[i][j] = k;
                        if (solve_sudoku_mpi(s, rank, size)) {
                            return 1;
                        }
                        s->grid[i][j] = UNASSIGNED;
                    }
                }
            }
        }
    }
    return 0;
}

