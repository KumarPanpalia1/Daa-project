# Library Search Benchmark

This project contains a C++ implementation of two search algorithms used for a dataset of accession numbers:

- **Linear search**
- **Binary search**

The program generates a random dataset, sorts it, and benchmarks search performance across best, average, and worst-case scenarios.

## Files

- `library_search.cpp` - main program source code
- `output.txt` - generated run output / benchmark results

## Build

Use a C++ compiler that supports C++17. For example:

```bash
g++ -std=c++17 library_search.cpp -o library_search
```

## Run

```bash
./library_search
```

## Notes

- The program measures both comparisons and elapsed time for each algorithm.
- `output.txt` is included as the output artifact from a sample run.
