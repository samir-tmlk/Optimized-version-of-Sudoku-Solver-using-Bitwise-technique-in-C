CC = gcc
CFLAGS = -Wall -Wextra -O3
TARGET1 = sudoku_solver_optimized
TARGET2 = sudoku_solver
SRC1 = sudoku_optimized.c
SRC2 = sudoku_solver_naive.c

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(SRC1)
	$(CC) $(CFLAGS) -o $(TARGET1) $(SRC1)

$(TARGET2): $(SRC2)
	$(CC) $(CFLAGS) -o $(TARGET2) $(SRC2)

clean:
	rm -f $(TARGET1) $(TARGET2)

run: $(TARGET1)
	./$(TARGET1) sudoku.txt

compare: $(TARGET1) $(TARGET2)
	@echo "=== Starting High-Precision Comparison ==="
	@echo "Note: Running each 1000 times to get a better average."
	@echo ""
	@echo "--- Naive Solver ---"
	@time (for i in {1..1000}; do ./$(TARGET2) sudoku.txt > /dev/null; done)
	@echo ""
	@echo "--- Optimized Solver ---"
	@time (for i in {1..1000}; do ./$(TARGET1) sudoku.txt > /dev/null; done)
	@echo ""
	@echo "Divide the 'user' time by 1000 to get the true average per run."
