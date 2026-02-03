#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Bitmask arrays to track which numbers (1-9) are used in each row, column, and 3x3 box.
int rows[9] = {0};
int cols[9] = {0};
int boxes[3][3] = {0};

// Fancy printing to make the Sudoku board look readable in the terminal
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

// Flip a bit to register or remove a number from our row/col/box tracking.
// We use XOR (^) so the same function works for both adding and backtracking.
void toggle_constraint(int r, int c, int num) {
    int bit = 1 << num;       // Map number 3 to 00001000, for example
    rows[r] ^= bit;           
    cols[c] ^= bit;
    boxes[r/3][c/3] ^= bit;
}

// Check if a number is already taken in the current row, column, or box using bitwise AND.
bool is_safe_bitwise(int r, int c, int num) {
    int bit = 1 << num;
    // If the bit is already set in any of these, the move is invalid.
    if ((rows[r] & bit)||(cols[c] & bit)||(boxes[r/3][c/3] & bit)) {
        return false;
    }
    return true;
}

// Load the grid from a text file and initialize our bitmasks
void read_grid(const char *path, int grid[9][9]){
    FILE *f = fopen(path,"r");
    if (f == NULL) {
        printf("Error: Could not open file %s\n", path);
        exit(1);
    }
    for (int i = 0; i < 9; i++){
        for (int j = 0; j < 9; j++){
            // Handle CSV or space-separated values
            if (fscanf(f, "%d,", &grid[i][j]) != 1) { 
                fseek(f, -1, SEEK_CUR); 
                if (fscanf(f, "%d", &grid[i][j]) != 1) {
                     // Basic error handling could go here
                }
            }
            // If the cell isn't empty, we need to "lock in" that number in our masks
            if (grid[i][j] != 0) {
                toggle_constraint(i, j, grid[i][j]);
            }
        }
    }
    fclose(f);
}

// Simple linear scan to find the next zero-value cell
bool find_empty_location(int grid[9][9], int *row, int *col) {
    for (*row = 0; *row < 9; (*row)++) {
        for (*col = 0; *col < 9; (*col)++) {
            if (grid[*row][*col] == 0) return true;
        }
    }
    return false;
}

// The core recursive backtracking engine
bool solve(int grid[9][9]){
    int row, col;
    
    // If no empty spots are left, the puzzle is solved!
    if (!find_empty_location(grid, &row, &col)) return true;

    for (int num = 1; num <= 9; num++){
        // Fast bitwise check before attempting a move
        if (is_safe_bitwise(row, col, num)){
            
            grid[row][col] = num;
            toggle_constraint(row, col, num); // Set the bit

            if (solve(grid)) return true;

            // Backtrack: If it didn't work, unset the bit and reset the cell
            toggle_constraint(row, col, num); 
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
    // Safety first: zero out the tracking arrays
    for(int i=0; i<9; i++) { 
        rows[i]=0; 
        cols[i]=0; 
    }
    for(int i=0; i<3; i++) {
        for(int j=0; j<3; j++) { 
            boxes[i][j]=0;
        }
    }

    printf("Loading puzzle from '%s'...\n", argv[1]);
    read_grid(argv[1], grid); 
    print(grid);

    printf("\nSolving with Bitwise Optimization...\n");
    if (solve(grid)) {
        printf("\n Solved Grid:\n");
        print(grid);
    } else {
        printf("\n No solution exists for this puzzle.\n");
    }

    return 0;
}