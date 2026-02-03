# Optimized-version-of-Sudoku-Solver-using-Bitwise-technique-in-C
A performance-focused Sudoku solver in C comparing standard backtracking against an optimized bitwise approach inspired by Google research. Features a comparative benchmark suite to measure $O(1)$ constraint checking efficiency.

This README is designed to tell the story of your project—from the initial "naive" solution to the "bitwise" optimization—while keeping the tone professional yet approachable.

---

# 🧩 Sudoku Solver: From Naive Backtracking to Bitwise Optimization

A high-performance C-based Sudoku solver that explores the efficiency gap between standard array-scanning and bitmask-based constraint checking.

## 📖 The Story

### Phase 1: The Naive Approach

I started this project by building a classic Sudoku solver. It used a standard **Backtracking algorithm**: find an empty cell, try a number, and manually scan the current row, column, and 3x3 square to see if that number was valid. It worked perfectly, but I wanted to see if I could make the "is it safe?" check faster than a manual  scan.

### Phase 2: The "Bitwise" Spark

While researching ways to optimize constraint satisfaction, I stumbled upon logic (inspired by techniques often discussed in performance-focused research, including some Google technical papers) regarding **Bitwise operations**.

The idea was elegant: instead of scanning arrays, why not represent the numbers 1–9 as bits in a single integer?

* **Checking a constraint** becomes a single bitwise `AND`.
* **Updating a constraint** becomes a single bitwise `XOR`.

This moved the validation check from a loop-based scan to a **constant time  operation**.

---

## 🚀 Performance Benchmarking

To accurately measure the difference, I ran both solvers through 1,000 iterations to dilute OS overhead and warm up the CPU cache.

### Results

| Approach | Avg. User Time (per run) | Complexity |
| --- | --- | --- |
| **Naive** | ~0.64 ms |  scan per check |
| **Optimized** | **~0.54 ms** | ** bitwise check** |

---

## 🛠️ Installation & Usage

### Prerequisites

* `gcc` (or any C compiler)
* `make`

### Build

To compile both versions of the solver:

```bash
make all

```

### Run Comparison

To see the performance difference on your own machine (runs 1,000 iterations):

```bash
make compare

```

---

## 💡 Final Thoughts: Is it worth it?

You might notice that the time saved is only about **0.1ms per puzzle**. While the bitwise approach is technically "faster" and much more elegant from a computer science perspective, the total time won isn't massive in this specific context.

**Why?** Sudoku is a  grid—a relatively small search space. For a problem of this scale, the overhead of function calls and the recursion stack often outweighs the time saved by bitwise math. The optimization truly shines on larger grids ( or ) or on "World's Hardest" puzzles where the algorithm has to backtrack millions of times.

In the end, this project was a great exercise in seeing how low-level data representation can squeeze extra juice out of a CPU!
