#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

// Helper to draw the board in the console so we can actually see what's happening
void print(int grid[9][9]) {
    printf("+--------+----------+----------+\n");
    for (int i = 0; i < 9; i++) {
        if (i != 0 && i % 3 == 0) printf("+--------+----------+----------+\n");
        for (int j = 0; j < 9; j++) {
            if (j != 0 && j % 3 == 0) printf("| ");
            if (grid[i][j] == 0) printf(" . "); // Print a dot for empty cells
            else printf(" %d ", grid[i][j]);
        }
        printf("|\n");
    }
    printf("+--------+----------+----------+\n");
}

// Load the Sudoku puzzle from a file (expects 81 integers)
void read_grid(const char *path, int grid[9][9]){
    FILE *f = fopen(path,"r");
    if (f == NULL) {
        printf("Error: Could not open file %s\n", path);
        exit(1);
    }
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            // Expecting comma-separated values, but fscanf is flexible
            if (fscanf(f, "%d,", &grid[i][j]) != 1) {
                printf("Error reading grid format.\n");
                fclose(f);
                exit(1);
            }
        }
    }
    fclose(f);
}

// Scan the current row to see if 'vtest' is already there
bool check_row(int grid[9][9], int row, int column, int vtest) {
    for (int j = 0; j < 9; j++) {
        if (j != column && grid[row][j] == vtest) return false;
    }
    return true;
}

// Scan the current column for the same number
bool check_column(int grid[9][9], int row, int column, int vtest) {
    for (int i = 0; i < 9; i++) {
        if (i != row && grid[i][column] == vtest) return false;
    }
    return true;
}

// Scan the 3x3 sub-grid (box) that this cell belongs to
bool check_square(int grid[9][9], int row, int column, int vtest) {
    // Find the top-left corner of the 3x3 box
    int startRow = 3 * (row / 3);
    int startColumn = 3 * (column / 3);
    
    for (int i = startRow; i < startRow + 3; i++) {
        for (int j = startColumn; j < startColumn + 3; j++) {
            if ((i != row || j != column) && grid[i][j] == vtest) return false;
        }
    }
    return true;
}

// Combine all three Sudoku rules: row, column, and box
bool check(int grid[9][9], int row, int col, int val) {
    return check_row(grid, row, col, val) &&
           check_column(grid, row, col, val) &&
           check_square(grid, row, col, val);
}

// Look for the first 0 in the grid from top-left to bottom-right
bool find_empty_location(int grid[9][9], int *row, int *col) {
    for (*row = 0; *row < 9; (*row)++) {
        for (*col = 0; *col < 9; (*col)++) {
            if (grid[*row][*col] == 0) return true;
        }
    }
    return false;
}

// Standard backtracking algorithm
bool solve(int grid[9][9]){
    int row, col;
    
    // Base case: if no empty spots are left, we're done!
    if (!find_empty_location(grid, &row, &col)) return true;

    // Try placing numbers 1 through 9
    for (int num = 1; num <= 9; num++){
        if (check(grid, row, col, num)){
            // Make the move
            grid[row][col] = num;

            // Recurse to see if this leads to a solution
            if (solve(grid)) return true;

            // If it didn't work, undo the move (backtrack) and try the next number
            grid[row][col] = 0; 
        }
    }
    return false; 
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.txt>\n", argv[0]);
        return 1;
    }

    int grid[9][9];
    
    printf("Loading puzzle from '%s'...\n", argv[1]);
    read_grid(argv[1], grid);
    print(grid);

    printf("\nSolving...\n");
    if (solve(grid)) {
        printf("\n Solved Grid:\n");
        print(grid);
    } else {
        printf("\n No solution exists for this puzzle.\n");
    }

    return 0;
}
