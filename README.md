# Sudoku Solver: From Naive Backtracking to Bitwise Constraint Propagation

Two Sudoku solvers in C, kept side by side in the same repository so the optimisation can be measured rather than asserted:

- `sudoku_solver_naive.c` — classic backtracking. Validity is checked by scanning the row, the column and the 3×3 box.
- `sudoku_optimized.c` — same search, but the constraint check is a single bitwise `AND` against three bitmask arrays, and updating/undoing a move is a single `XOR`.

The point of the repo is the **benchmark methodology**, not the solver: naive backtracking already solves a 9×9 grid in under a millisecond, so isolating the gain requires being careful about what is actually being timed.

---

## The idea

In the naive solver, asking *"can I place `n` at `(r, c)`?"* means walking 9 cells of the row, 9 of the column and 9 of the box — up to 27 comparisons, executed at every node of the search tree.

The optimised solver keeps three sets of bitmasks:

```c
int rows[9], cols[9], boxes[3][3];   // bit k set  <=>  digit k already used
```

The digit `n` is represented as the bit `1 << n`. Then:

```c
// "Is this move legal?"  -> one AND per constraint, no loop
if ((rows[r] & bit) || (cols[c] & bit) || (boxes[r/3][c/3] & bit)) return false;

// "Place" and "undo" are the same operation: XOR is its own inverse
rows[r] ^= bit;  cols[c] ^= bit;  boxes[r/3][c/3] ^= bit;
```

The validity test drops from an O(1)-but-27-comparison scan to three register operations, and backtracking becomes free — no state has to be recomputed or restored, because `toggle_constraint` undoes itself.

Both solvers explore **the same search tree in the same order** (linear scan for the next empty cell, digits tried 1→9). The algorithm is unchanged; only the cost of the inner test changes. That is deliberate: it makes the measurement attributable to register-level work rather than to a better search.

---

## Benchmark

### Methodology

`make compare` runs each binary 1,000 times in a shell loop. That loop measures **process spawn + file parsing + grid printing + solving**, and on modern hardware the spawn dominates. To separate the signal from that floor, the numbers below include a **no-op baseline**: an empty C program (`int main(){return 0;}`) compiled with the same flags and run through the identical 1,000-iteration loop.

Environment: **Apple M2, macOS 26.5, Apple clang 17.0.0, `-Wall -Wextra -O3`**. Median of 3 × 1,000 runs on `sudoku.txt`.

### Results

| Binary | Total, 1,000 runs | Per run |
|---|---:|---:|
| No-op baseline (spawn floor) | 1.796 s | 1.80 ms |
| Naive backtracking | 2.134 s | 2.13 ms |
| Bitwise constraint propagation | 2.009 s | 2.01 ms |

**End-to-end: 5.9% faster** (2.134 s → 2.009 s).

Subtracting the spawn floor leaves the work the program actually does:

| | Work above the floor, 1,000 runs |
|---|---:|
| Naive | 0.338 s |
| Optimized | 0.213 s |
| | **1.58× faster, −37%** |

### How to read this honestly

- The headline "5.9%" and the "−37%" describe the same change measured against different denominators. Neither is wrong; quoting one without the other is.
- Even the 0.213 s residual is **not** pure solve time — it still contains `fopen`/`fscanf` and two full `printf` grid dumps per run. The true solver-only speed-up is therefore **larger than 1.58×**, but this harness cannot resolve it further.
- The residual (~0.2–0.3 s) is small relative to run-to-run noise (~±0.05 s), so treat these as one significant figure. Anyone wanting a defensible number should time `solve()` in-process with `clock_gettime` across many puzzles, rather than timing the process.
- Results are hardware- and puzzle-dependent. `sudoku.txt` is a single moderately-constrained grid; a puzzle engineered to force millions of backtracks would widen the gap substantially, because the constraint check is executed once per search-tree node.

### Reproduce

```bash
cd Code
make all
make compare
```

`make compare` prints total user time for 1,000 iterations of each solver — divide by 1,000 for the per-run average. To reproduce the baseline correction:

```bash
printf 'int main(){return 0;}' > /tmp/noop.c && gcc -O3 -o /tmp/noop /tmp/noop.c
time ( for i in $(seq 1 1000); do /tmp/noop; done )
```

---

## Usage

### Build

```bash
cd Code
make all          # builds sudoku_solver (naive) and sudoku_solver_optimized
```

### Solve a puzzle

```bash
./sudoku_solver_optimized sudoku.txt
# or
make run
```

Both binaries print the input grid, solve it, and print the result.

### Puzzle format

Nine lines of nine values, comma- or space-separated, `0` for an empty cell:

```
5, 0, 0, 0, 6, 0, 0, 0, 0
0, 0, 3, 0, 9, 8, 0, 0, 2
0, 6, 0, 1, 0, 2, 0, 0, 9
...
```

---

## Repository layout

```
Code/
  sudoku_solver_naive.c    Backtracking with row/column/box scanning
  sudoku_optimized.c       Backtracking with bitmask constraint propagation
  Makefile                 all / run / compare / clean
  sudoku.txt               Benchmark puzzle
```

## Known limitations

- The next empty cell is found by a linear rescan of the whole grid from `(0,0)` at every recursion level. Tracking the position, or better, selecting the **most-constrained cell first** (minimum remaining values), would cut far more search than the bitwise trick does — the constraint check is a constant factor, MRV changes the size of the tree.
- Bitmasks are file-scope globals, so the solver is not reentrant or thread-safe.
- `read_grid` swallows malformed input silently; a grid that fails to parse is solved as if the bad cells were empty.
- No verification that the input puzzle is well-formed or uniquely solvable.

## Reference

The bitmask constraint-propagation technique follows the approach described in McGuire, Tugemann & Civario, *"There is no 16-Clue Sudoku"* (2012), which uses bit-parallel set representations for exhaustive Sudoku search.
