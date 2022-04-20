/**
 * @author Benjamin Peronne
 * @email contact@benjaminperonne.fr
 * @create date 2022-03-04 17:45:00
 * @modify date 2022-03-04 17:45:00
 * @desc [Sudoku function]
 */

typedef struct sudoku { // Sudoku struct definition 
    int **grid;
    int size;
} sudoku;

void sudoku_init(sudoku *s, int size) { // Initialize the sudoku grid with 0 (empty)
    s->size = size;
    s->grid = malloc(sizeof(int *) * size);
    for (int i = 0; i < size; i++) {
        s->grid[i] = malloc(sizeof(int) * size);
    }
}
void sudoku_free(sudoku *s) {

void sudoku_print(sudoku *s){ // Print the sudoku grid
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


